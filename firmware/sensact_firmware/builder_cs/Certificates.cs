// Portierung von certificates.ts (npm-packages/@klaus-liebler/espidf-vite-secure-build-tools),
// bewusst NICHT 1:1 (node-forge -> System.Security.Cryptography.X509Certificates), sondern nativ
// neu gebaut -- s. docs/plan_v2/02-builder-migration-csharp.md, "Entschieden": .NET bringt mit
// CertificateRequest/X509Certificate2 eine vollwertige native X.509-Implementierung mit, kein
// Shell-out zu openssl noetig (anders als im Referenzprojekt, das openssl per Prozessaufruf nutzt).
//
// Erzeugt dieselben drei Zertifikatsarten wie das TS-Original: selbstsignierte Root-CA, davon
// signierte "Universal-Auth"-Leaf-Zertifikate (Board/Server, mit IP+mehreren DNS-SANs) und
// Client-Zertifikate (fixe IP 192.168.4.1, ein DNS-SAN = Username). Alle RSA-2048/SHA-256, 3000
// Tage Gueltigkeit (minus 1 Tag Vorlauf wegen Client-Uhr-Skew) -- Werte 1:1 aus dem TS-Original
// uebernommen.
using System.Security.Cryptography;
using System.Security.Cryptography.X509Certificates;

namespace Builder;

public static class Certificates
{
	// Custom-OID aus certificates.ts (per https://freeoid.pythonanywhere.com/getoid erzeugt) --
	// Flag-Extension fuer den Labathome-Webservice-Zugriff. Im TS-Original IMMER auf den festen
	// UTF8-Bytes "false" gesetzt (kein Parameter beeinflusst das) -- 1:1 so uebernommen, auch wenn
	// das wie ein nie fertiggestelltes Feature aussieht (bewusst nicht "korrigiert").
	private const string CustomFlagOid = "1.3.6.1.4.1.54392.5.2757";

	public sealed record PemCertificate(string Certificate, string PrivateKey);

	// Parst BuilderOptions.Current.SubjectPrefix (OpenSSL-"-subj"-Notation, z.B.
	// "/C=DE/ST=NRW/L=Greven/O=Klaus Liebler personal") in ein .NET-X500DistinguishedName (RFC-2253-
	// Notation: umgekehrte Reihenfolge, kommagetrennt, CN zuerst) -- keine personenbezogenen Werte
	// im Code, s. BuilderOptions.cs.
	private static X500DistinguishedName BuildSubject(string commonName)
	{
		var prefix = BuilderOptions.Current.SubjectPrefix;
		var rdns = prefix.Split('/', StringSplitOptions.RemoveEmptyEntries);
		var dn = $"CN={commonName}" + (rdns.Length > 0 ? ", " + string.Join(", ", rdns.Reverse()) : "");
		return new X500DistinguishedName(dn);
	}

	// .NET erwartet den Seriennummer-Byte-Array in Little-Endian-Reihenfolge (das letzte Byte ist
	// das hoechstwertige) -- deshalb wird hier (anders als in certificates.ts, das Big-Endian-Hex
	// erzeugt und Byte 0 maskiert) das LETZTE Byte maskiert, um laut RFC 5280 eine garantiert
	// positive ASN.1-INTEGER-Seriennummer zu erhalten.
	private static byte[] RandomSerialNumber(int numberOfBytes)
	{
		var buf = RandomNumberGenerator.GetBytes(numberOfBytes);
		buf[^1] &= 0x7F;
		return buf;
	}

	private static X509BasicConstraintsExtension RootCaBasicConstraints() =>
		new(certificateAuthority: true, hasPathLengthConstraint: false, pathLengthConstraint: 0, critical: true);

	private static X509KeyUsageExtension RootCaKeyUsage() =>
		new(X509KeyUsageFlags.KeyCertSign | X509KeyUsageFlags.CrlSign, critical: true);

	// Entspricht createUniversalAuthExtensions() in certificates.ts.
	private static void AddLeafExtensions(CertificateRequest req, X509Certificate2 caCert, string ipAddressAsString, IReadOnlyList<string> dnsHostnames)
	{
		// forge's eigenes "authorityKeyIdentifier" setzt NUR authorityCertIssuer+serialNumber (kein
		// keyIdentifier) -- passend dazu hat die echte, produktiv genutzte Root-CA selbst KEINE
		// Subject-Key-Identifier-Extension (s. createRootCaExtensions() in certificates.ts: nur
		// basicConstraints+keyUsage). includeKeyIdentifier:true wuerde deshalb hier scheitern
		// ("certificate does not have a Subject Key Identifier extension").
		req.CertificateExtensions.Add(X509AuthorityKeyIdentifierExtension.CreateFromCertificate(caCert, includeKeyIdentifier: false, includeIssuerAndSerial: true));
		req.CertificateExtensions.Add(new X509BasicConstraintsExtension(certificateAuthority: false, hasPathLengthConstraint: false, pathLengthConstraint: 0, critical: false));
		req.CertificateExtensions.Add(new X509KeyUsageExtension(
			X509KeyUsageFlags.DigitalSignature | X509KeyUsageFlags.NonRepudiation | X509KeyUsageFlags.KeyEncipherment | X509KeyUsageFlags.DataEncipherment,
			critical: false));
		req.CertificateExtensions.Add(new X509EnhancedKeyUsageExtension(
			[new Oid("1.3.6.1.5.5.7.3.1"), new Oid("1.3.6.1.5.5.7.3.2")], // serverAuth, clientAuth
			critical: false));
		req.CertificateExtensions.Add(new X509SubjectKeyIdentifierExtension(req.PublicKey, X509SubjectKeyIdentifierHashAlgorithm.Sha1, critical: false));
		req.CertificateExtensions.Add(new X509Extension(new Oid(CustomFlagOid), "false"u8.ToArray(), critical: false));

		var sanBuilder = new SubjectAlternativeNameBuilder();
		sanBuilder.AddIpAddress(System.Net.IPAddress.Parse(ipAddressAsString));
		foreach (var dnsHostname in dnsHostnames)
		{
			sanBuilder.AddDnsName(dnsHostname);
		}
		req.CertificateExtensions.Add(sanBuilder.Build());
	}

	private static PemCertificate SignLeaf(X500DistinguishedName subject, X509Certificate2 caCert, Action<CertificateRequest> addExtensions)
	{
		using var rsa = RSA.Create(2048);
		var req = new CertificateRequest(subject, rsa, HashAlgorithmName.SHA256, RSASignaturePadding.Pkcs1);
		addExtensions(req);

		var notBefore = DateTimeOffset.UtcNow.AddDays(-1);
		var notAfter = DateTimeOffset.UtcNow.AddDays(3000);
		using var signed = req.Create(caCert, notBefore, notAfter, RandomSerialNumber(20));
		using var withKey = signed.CopyWithPrivateKey(rsa);
		return new PemCertificate(withKey.ExportCertificatePem(), rsa.ExportPkcs8PrivateKeyPem());
	}

	public static PemCertificate CreateRootCA(string commonName)
	{
		using var rsa = RSA.Create(2048);
		var req = new CertificateRequest(BuildSubject(commonName), rsa, HashAlgorithmName.SHA256, RSASignaturePadding.Pkcs1);
		req.CertificateExtensions.Add(RootCaBasicConstraints());
		req.CertificateExtensions.Add(RootCaKeyUsage());

		var notBefore = DateTimeOffset.UtcNow.AddDays(-1);
		var notAfter = DateTimeOffset.UtcNow.AddDays(3000);
		using var cert = req.CreateSelfSigned(notBefore, notAfter);
		return new PemCertificate(cert.ExportCertificatePem(), rsa.ExportPkcs8PrivateKeyPem());
	}

	public static PemCertificate CreateAndSignCert(string commonName, string ipAddressAsString, IReadOnlyList<string> dnsHostnames, string certificateCaPemPath, string privateKeyCaPemPath)
	{
		using var caCert = X509Certificate2.CreateFromPemFile(certificateCaPemPath, privateKeyCaPemPath);
		return SignLeaf(BuildSubject(commonName), caCert, req => AddLeafExtensions(req, caCert, ipAddressAsString, dnsHostnames));
	}

	public static PemCertificate CreateAndSignClientCert(string username, string certificateCaPemPath, string privateKeyCaPemPath)
	{
		using var caCert = X509Certificate2.CreateFromPemFile(certificateCaPemPath, privateKeyCaPemPath);
		return SignLeaf(BuildSubject(username), caCert, req => AddLeafExtensions(req, caCert, "192.168.4.1", [username]));
	}
}
