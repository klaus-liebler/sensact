using System.Xml;
using System.Xml.Linq;
using System.Xml.Schema;

namespace SensactEditor.Services;

/// <summary>Art eines App-Attributs, bestimmt das Eingabefeld im Formular.</summary>
public enum FieldKind { Text, Number, Boolean, Choice, AppRef, AppRefList, IoRef, IoRefList }

public sealed record FieldInfo(
	string Name,
	FieldKind Kind,
	bool Required,
	string? Default,
	IReadOnlyList<string> Choices,
	string? Doc,
	IoDirectionHint Direction);

public enum IoDirectionHint { None, Input, Output }

public sealed record AppTypeInfo(string Element, string? Doc, IReadOnlyList<FieldInfo> Fields);

/// <summary>
/// Liest model/schema/sensact-house.xsd und beschreibt jeden App-Typ (Elemente der Gruppe "Apps")
/// mit seinen Attributen -- daraus baut der Editor die Formulare, statt sie pro App von Hand zu
/// programmieren. Beschreibungstexte kommen aus den XML-Kommentaren direkt vor dem Typ bzw.
/// Attribut im XSD.
/// </summary>
public sealed class AppSchema
{
	/// <summary>Attribute, die jede App hat und die der Editor gesondert behandelt</summary>
	public static readonly string[] CommonAttributes = ["id", "number", "description"];

	public IReadOnlyDictionary<string, AppTypeInfo> Types { get; }

	public AppSchema(string xsdPath)
	{
		var set = new XmlSchemaSet();
		set.Add(null, xsdPath);
		set.Compile();
		var schema = set.Schemas().Cast<XmlSchema>().Single();
		var docs = ReadComments(xsdPath);

		var group = (XmlSchemaGroup)schema.Groups[new XmlQualifiedName("Apps")]!;
		var types = new Dictionary<string, AppTypeInfo>(StringComparer.Ordinal);
		foreach (XmlSchemaElement el in ((XmlSchemaChoice)group.Particle!).Items)
		{
			var ct = (XmlSchemaComplexType)el.ElementSchemaType!;
			string typeName = ct.QualifiedName.Name;
			var fields = new List<FieldInfo>();
			foreach (var name in AttributeOrder(ct))
			{
				var au = (XmlSchemaAttribute)ct.AttributeUses[new XmlQualifiedName(name)]!;
				fields.Add(Describe(au, docs.TryGetValue((typeName, name), out var d) ? d : null));
			}
			types[el.Name!] = new AppTypeInfo(el.Name!, docs.TryGetValue((typeName, ""), out var td) ? td : null, fields);
		}
		Types = types;
	}

	/// <summary>Attribute in Deklarationsreihenfolge: erst die der Basis "App", dann die der Ableitung.</summary>
	private static IEnumerable<string> AttributeOrder(XmlSchemaComplexType ct)
	{
		var result = new List<string>();
		void Collect(XmlSchemaComplexType t)
		{
			if (t.BaseXmlSchemaType is XmlSchemaComplexType b && b.QualifiedName.Name != "anyType") Collect(b);
			XmlSchemaObjectCollection? attrs = t.ContentModel?.Content switch
			{
				XmlSchemaComplexContentExtension ext => ext.Attributes,
				_ => t.Attributes,
			};
			foreach (var a in attrs.OfType<XmlSchemaAttribute>())
			{
				if (!result.Contains(a.Name!)) result.Add(a.Name!);
			}
		}
		Collect(ct);
		return result;
	}

	private static FieldInfo Describe(XmlSchemaAttribute au, string? doc)
	{
		var st = au.AttributeSchemaType!;
		string typeName = st.QualifiedName.Name;
		var choices = new List<string>();
		for (var t = st; t != null; t = t.BaseXmlSchemaType as XmlSchemaSimpleType)
		{
			if (t.Content is XmlSchemaSimpleTypeRestriction r)
			{
				choices.AddRange(r.Facets.OfType<XmlSchemaEnumerationFacet>().Select(f => f.Value!));
				if (choices.Count > 0) break;
			}
		}
		string name = au.QualifiedName.Name;
		var dir = name.StartsWith("input", StringComparison.Ordinal) ? IoDirectionHint.Input : IoDirectionHint.Output;
		FieldKind kind = typeName switch
		{
			"IoRef" => FieldKind.IoRef,
			"IoRefList" => FieldKind.IoRefList,
			"AppRefList" => FieldKind.AppRefList,
			_ when choices.Count > 0 => FieldKind.Choice,
			_ when st.Datatype!.TypeCode == XmlTypeCode.Idref && st.Datatype.Variety == XmlSchemaDatatypeVariety.List => FieldKind.AppRefList,
			_ => st.Datatype!.TypeCode switch
			{
				XmlTypeCode.Idref => FieldKind.AppRef,
				XmlTypeCode.Boolean => FieldKind.Boolean,
				XmlTypeCode.Decimal or XmlTypeCode.Integer or XmlTypeCode.Int or XmlTypeCode.Short or XmlTypeCode.UnsignedShort
					or XmlTypeCode.UnsignedInt or XmlTypeCode.UnsignedByte or XmlTypeCode.PositiveInteger => FieldKind.Number,
				_ => FieldKind.Text,
			},
		};
		return new FieldInfo(name, kind, au.Use == XmlSchemaUse.Required, au.DefaultValue, choices, doc,
			kind is FieldKind.IoRef or FieldKind.IoRefList ? dir : IoDirectionHint.None);
	}

	/// <summary>(Typname, Attributname oder "") -> Kommentar unmittelbar davor im XSD</summary>
	private static Dictionary<(string, string), string> ReadComments(string xsdPath)
	{
		XNamespace xs = XmlSchema.Namespace;
		var doc = XDocument.Load(xsdPath);
		var result = new Dictionary<(string, string), string>();
		static string? CommentBefore(XElement e) =>
			e.NodesBeforeSelf().Reverse().SkipWhile(n => n is XText).FirstOrDefault() is XComment c ? Clean(c.Value) : null;
		foreach (var ct in doc.Root!.Elements(xs + "complexType"))
		{
			string typeName = (string)ct.Attribute("name")!;
			if (CommentBefore(ct) is string d) result[(typeName, "")] = d;
			foreach (var a in ct.Descendants(xs + "attribute"))
			{
				if (CommentBefore(a) is string ad) result[(typeName, (string)a.Attribute("name")!)] = ad;
			}
		}
		return result;
	}

	private static string Clean(string comment) =>
		string.Join(' ', comment.Split('\n').Select(l => l.Trim()).Where(l => l.Length > 0));
}
