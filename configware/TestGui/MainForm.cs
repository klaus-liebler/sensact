using Klli.Sensact.Common;
using Klli.Sensact.Config;
using MiscUtil.Conversion;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO.Ports;
using System.Reflection;
using System.Windows.Forms;

namespace Klli.Sensact.TestGui
{


    public partial class MainForm : Form
    {
        private class TreeHelper
        {
            public string Name; //internal
            public string Text; //GUI
            public Dictionary<string, TreeHelper> Children = new Dictionary<string, TreeHelper>();

            public void Append(string fullName, string remainingName, char separator)
            {
                int pos = remainingName.IndexOf(separator);
                if (pos!=-1)
                {
                    string childsName = remainingName.Substring(0, pos);
                    string nextName = remainingName.Substring(pos + 1);
                    TreeHelper child;
                    if(!Children.TryGetValue(childsName, out child))
                    {
                        child = new TreeHelper{ Text = childsName };
                        Children.Add(childsName, child);
                    }
                    child.Append(fullName, nextName, separator);
                }
                else
                {
                    string childsName = remainingName;
                    TreeHelper child=new TreeHelper { Text = childsName, Name=fullName };
                    Children.Add(childsName, child);
                }
            }

            public TreeNode CreateTreeNodeHierarchy()
            {
                TreeNode[] childrenArr = new TreeNode[this.Children.Count];
                int i = 0;
                foreach(var child in Children.Values)
                {
                    childrenArr[i] = child.CreateTreeNodeHierarchy();
                    i++;
                }
                TreeNode tn = new TreeNode(Text, childrenArr);
                tn.Name = Name;
                return tn;
            }
        }

        private class Line
        {
            public string Str;
            public Color ForeColor;

            public Line(string str, Color color)
            {
                Str = str;
                ForeColor = color;
            }
        };
        private Dictionary<string, CommandSpecification> name2cmdSpec = new Dictionary<string, CommandSpecification>();
        private ModelContainer mc;
        private CommPort commPort;
        public MainForm()
        {
            InitializeComponent();
            lisTerminal.DrawMode = DrawMode.OwnerDrawFixed;
            lisTerminal.DrawItem += new DrawItemEventHandler(lisTerminal_DrawItem);
            cboSelectCOM.DataSource = SerialPort.GetPortNames();
            commPort = new CommPort();
            mc = Config.Program.CreateAndCheckModelContainer();
            if(mc!=null)
            {
                TreeHelper root = new TreeHelper { Text = mc.Model.Name };
                foreach (var app in mc.id2app.Values)
                {
                    root.Append(app.Application.ApplicationId, app.Application.ApplicationId, '_');
                }
                treeView1.Nodes.Add(root.CreateTreeNodeHierarchy());

            }

        }

        void lisTerminal_DrawItem(object sender, DrawItemEventArgs e)
        {
            e.DrawBackground();
            if (e.Index >= 0 && e.Index < lisTerminal.Items.Count)
            {
                Line line = (Line)lisTerminal.Items[e.Index];

                // if selected, make the text color readable
                Color color = line.ForeColor;
                if ((e.State & DrawItemState.Selected) == DrawItemState.Selected)
                {
                    color = Color.Black;    // make it readable
                }

                e.Graphics.DrawString(line.Str, e.Font, new SolidBrush(color),
                    e.Bounds, StringFormat.GenericDefault);
            }
            e.DrawFocusRectangle();
        }

        private Control CreateCommandsTable(SensactApplicationContainer sac)
        {
            
            Type t = sac.Application.GetType();
            int row = 0;
            foreach (MethodInfo m in t.GetMethods())
            {
                if (m.GetCustomAttribute<SensactCommandMethod>() != null && m.IsOverride())
                {
                    row++;
                }
            }
            if(row==0)
            {
                return new Label
                {
                    Text = "NO COMMANDS DEFINED",
                    Location = new Point(299, 41),
                    Name = "pnlMaster",
                    Size = new Size(673, 408),
                };
            }
            TableLayoutPanel commandsTable = new TableLayoutPanel
            {
                RowCount = row,
                ColumnCount = 2,
                CellBorderStyle = TableLayoutPanelCellBorderStyle.Single,
                AutoSize=true,
                AutoSizeMode=AutoSizeMode.GrowAndShrink,
                AutoScroll = true,
                Location = new Point(299, 41),
                Name = "pnlMaster",
                Size = new Size(673, 408),
            };
            commandsTable.ColumnStyles.Add(new ColumnStyle(SizeType.Absolute, 200));
            commandsTable.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));
            row = 0;
            foreach (MethodInfo m in t.GetMethods())
            {
                if (m.GetCustomAttribute<SensactCommandMethod>() != null && m.IsOverride())
                {
                    CreateOneRow(sac, m, commandsTable, row);
                    row++;
                }
            }
            return commandsTable;
        }

        public static bool IsDecimalType(Type t)
        {
            TypeCode tc = Type.GetTypeCode(t);
            switch (tc)
            {
                case TypeCode.Byte:
                case TypeCode.SByte:
                case TypeCode.UInt16:
                case TypeCode.UInt32:
                case TypeCode.UInt64:
                case TypeCode.Int16:
                case TypeCode.Int32:
                case TypeCode.Int64:
                case TypeCode.Decimal:
                    return true;
                default:
                    return false;
            }
        }

        static RowStyle rs = new RowStyle(SizeType.Absolute, 40);

        private void CreateOneRow(SensactApplicationContainer sac, MethodInfo mi, TableLayoutPanel functionTable, int functionTableRow)
        {
            Button btn = CreateCommandButton(sac, mi);
            functionTable.Controls.Add(btn, 0, functionTableRow);
            functionTable.RowStyles.Add(new RowStyle(SizeType.Absolute, 40));
            Config.CommandType ct = Config.CommandType.NOP;
            if(!Enum.TryParse<Config.CommandType>(SensactApplication.ExtractCmdName(mi), out ct))
            {
                throw new Exception("Method name " + mi.Name + " that ist marked as SensactCommand cannot be parsed into a CommandType");
            }

            CommandSpecification cmdSpec = new CommandSpecification
            {
                applicationIdAsUshort = (ushort)sac.Index,
                CommandIdAsInt = (byte)ct,
            };
            FlowLayoutPanel flowLayoutPanel = new FlowLayoutPanel
            {
                FlowDirection = FlowDirection.LeftToRight
            };
            
            foreach (ParameterInfo p in mi.GetParameters())
            {
                flowLayoutPanel.Controls.Add(CreateParamNameLabel(p.Name));
                Control inp = null;
                if (IsDecimalType(p.ParameterType))
                {
                    inp = CreateDecimalParamInput(p);
                }
                else
                {
                    throw new NotSupportedException("Only decimal parameter types are supported");
                }
                flowLayoutPanel.Controls.Add(inp);
                cmdSpec.C2Ps.Add(new Control2Parameter { TheControl = inp, TheParameter = p });
            }
            name2cmdSpec[btn.Name] = cmdSpec;
            functionTable.Controls.Add(flowLayoutPanel, 1, functionTableRow);
        }

        private Control CreateDecimalParamInput(ParameterInfo p)
        {
            decimal max = 0;
            decimal min = 0;

            if (p.ParameterType == typeof(byte)) //uint8_t
            {
                max = byte.MaxValue;
                min = byte.MinValue;
            }
            else if (p.ParameterType == typeof(sbyte)) //int8_t
            {
                max = sbyte.MaxValue;
                min = sbyte.MinValue;
            }
            else if (p.ParameterType == typeof(ushort)) //uint16_t
            {
                max = ushort.MaxValue;
                min = ushort.MinValue;
            }
            else if (p.ParameterType == typeof(short)) //int16_t
            {
                max = short.MaxValue;
                min = short.MinValue;
            }
            else if (p.ParameterType == typeof(uint)) //uint32_t
            {
                max = uint.MaxValue;
                min = uint.MinValue;
            }
            else if (p.ParameterType == typeof(int)) //int32_t
            {
                max = int.MaxValue;
                min = int.MinValue;
            }

            return new NumericUpDown
            {
                Size = BTN_SIZE,
                Maximum=max,
                Minimum=min,
            };
        }

        private readonly Size BTN_SIZE = new Size(180, 23);
        private readonly Size LBL_SIZE_APP_NAME = new Size(150, 23);
        private readonly Size LBL_SIZE_PARAM = new Size(50, 23);

        private Button CreateCommandButton(SensactApplicationContainer sac, MethodInfo mi)
        {
            Button button=new Button();
            button.Size = BTN_SIZE;
            button.Text = SensactApplication.ExtractCmdName(mi);
            button.Name = sac.Application.ApplicationId+"_"+mi.Name;
            button.UseVisualStyleBackColor = true;
            button.Click += new System.EventHandler(cmdButton_Click);
            return button;
        }

        private Label CreateAppNameLabel(string appName)
        {
            return new Label
            {
                AutoSize = true,
                Name = "lbl" + appName,
                Size = LBL_SIZE_APP_NAME,
                Text = appName,
            };
        }

        private Label CreateParamNameLabel(string appName)
        {
            return new Label
            {
                AutoSize = true,
                Name = "lbl" + appName,
                Size = LBL_SIZE_PARAM,
                Text = appName,
            };
        }


        private void btnSetTime_Click(object sender, EventArgs e)
        {
            TimeSpan t = DateTime.UtcNow - new DateTime(1970, 1, 1);
            ulong secondsSinceEpoch = (ulong)t.TotalSeconds;
            byte[] buffer = new byte[100];
            buffer[0] = 0x01; //0x01==START_OF_HEADING
            buffer[1] = 12; 
            EndianBitConverter.Little.CopyBytes((ushort)0x03, buffer, 2); //SET_TIME
            EndianBitConverter.Little.CopyBytes(secondsSinceEpoch, buffer, 4);
            commPort.Write(buffer, 0, 12);
        }

        private void btnGetTime_Click(object sender, EventArgs e)
        {
            TimeSpan t = DateTime.UtcNow - new DateTime(1970, 1, 1);
            ulong secondsSinceEpoch = (ulong)t.TotalSeconds;
            byte[] buffer = new byte[100];
            buffer[0] = 0x01; //0x01==START_OF_HEADING
            buffer[1] = 4;
            EndianBitConverter.Little.CopyBytes((ushort)0x04, buffer, 2); //SET_TIME
            commPort.Write(buffer, 0,4);
        }



        private void cmdButton_Click(object sender, EventArgs e)
        {
            CommandSpecification cmdSpec = name2cmdSpec[(sender as Control).Name];
            byte[] buffer = new byte[100];
            buffer[0] = 0x01; //0x01==START_OF_HEADING
            buffer[1] = 0x00; // Length
            EndianBitConverter.Little.CopyBytes((ushort)0x00, buffer, 2); //WRITE_CAN
            EndianBitConverter.Little.CopyBytes(cmdSpec.applicationIdAsUshort, buffer, 4);
            byte pos = 6;
            buffer[pos] = cmdSpec.CommandIdAsInt;
            pos += 1;
            foreach (var cmd in cmdSpec.C2Ps)
            {
                AddParam(buffer, cmd, ref pos);
            }
            buffer[1] = pos;
            commPort.Write(buffer, 0, pos);
            return;

        }

        private void AddParam(byte[] buffer, Control2Parameter cmd, ref byte pos)
        {
            if(IsDecimalType(cmd.TheParameter.ParameterType))
            {
                NumericUpDown c = cmd.TheControl as NumericUpDown;
                ParameterInfo p = cmd.TheParameter;
                EndianBitConverter conv = EndianBitConverter.Little;
                if (p.ParameterType == typeof(byte)) //uint8_t
                {
                    buffer[pos] = (byte)c.Value;
                    pos += 1;
                }
                else if (p.ParameterType == typeof(sbyte)) //int8_t
                {
                    buffer[pos] = unchecked((byte)(c.Value));
                    pos += 1;
                }
                else if (p.ParameterType == typeof(ushort)) //uint16_t
                {
                    conv.CopyBytes((ushort)c.Value, buffer, pos);
                    pos += 2;
                }
                else if (p.ParameterType == typeof(short)) //int16_t
                {
                    conv.CopyBytes((short)c.Value, buffer, pos);
                    pos += 2;
                }
                else if (p.ParameterType == typeof(uint)) //uint32_t
                {
                    conv.CopyBytes((uint)c.Value, buffer, pos);
                    pos += 4;
                }
                else if (p.ParameterType == typeof(int)) //int32_t
                {
                    conv.CopyBytes((int)c.Value, buffer, pos);
                    pos += 4;
                }
            }
        }

        private void btnUpdateCOM_Click(object sender, EventArgs e)
        {
            cboSelectCOM.DataSource = SerialPort.GetPortNames();
        }

        private void btnOpenCOM_Click(object sender, EventArgs e)
        {
            if(commPort.IsOpen)
            {
                commPort.Close();
            }
            string port = cboSelectCOM.Text;
            commPort.PortName = port;
            commPort.Open();
            commPort.DataReceived += OnDataReceived;
        }
        internal delegate void StringDelegate(string data);
        private List<Line> lines = new List<Line>();

        public void OnDataReceived(string dataIn)
        {
            //Handle multi-threading
            if (InvokeRequired)
            {
                Invoke(new StringDelegate(OnDataReceived), new object[] { dataIn });
                return;
            }
            Line line = new Line(dataIn, GetColorOfLine(dataIn));
            lines.Add(line);
            lisTerminal.Items.Add(line);
            outputList_Scroll();
        }

        void outputList_Scroll()
        {
            
            int itemsPerPage = (int)(lisTerminal.Height / lisTerminal.ItemHeight);
            lisTerminal.TopIndex = lisTerminal.Items.Count - itemsPerPage;
        }

        private Color GetColorOfLine(string s)
        {
            switch (s[0])
            {
                case 'I': return Color.Green;
                case 'D': return Color.DarkGray;
                case 'W': return Color.Yellow;
                case 'E': return Color.Red;
                default: return Color.Black;
            }
        }


        private void treeView1_AfterSelect(object sender, TreeViewEventArgs e)
        {

            SensactApplicationContainer sac;
            if(!mc.id2app.TryGetValue(e.Node.Name, out sac))
            {
                return;
            }
            this.Controls.Remove(cntrlMaster);
            this.cntrlMaster.Dispose();
            this.cntrlMaster = CreateCommandsTable(sac);
            this.Controls.Add(cntrlMaster);
            Invalidate();
            this.Update();
            this.Refresh();


            return;
        }

        
    }
}
