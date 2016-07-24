using Klli.Sensact.Config;
using MiscUtil.Conversion;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Klli.Sensact.TestGui
{
    public partial class MainForm : Form
    {
        private TableLayoutPanel masterTlp;
        private Dictionary<string, CommandSpecification> name2cmdSpec = new Dictionary<string, CommandSpecification>();
        public MainForm()
        {
            InitializeComponent();

            this.serialPort.PortName = Properties.Settings.Default.SerialPort;
            //serialPort.Open();

            this.SuspendLayout();
            masterTlp = new TableLayoutPanel
            {
                AutoSize = true,
                AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink,
                GrowStyle = System.Windows.Forms.TableLayoutPanelGrowStyle.AddRows,
                Location = new System.Drawing.Point(13, 42),
                MaximumSize = new Size(1000, 400),
                CellBorderStyle = TableLayoutPanelCellBorderStyle.OutsetDouble,
                AutoScroll = true,
            };
           
            int row = 0;
            ModelContainer m = Config.Program.CreateAndCheckModelContainer();
            if(m!=null)
            {
                
                foreach (var app in m.id2app.Values)
                {
                    Label appIdLabel = new Label
                    {
                        Text = app.Application.ApplicationId,
                    };
                    masterTlp.Controls.Add(appIdLabel, 0, row++);
                    CreateCommandsTable(app, ref row);
                }
            }
            this.Controls.Add(masterTlp);
            this.ResumeLayout();

        }

        private void CreateCommandsTable(SensactApplicationContainer sac, ref int row)
        {
            Type t = sac.Application.GetType();

            TableLayoutPanel commandsTable=null;
            int functionTableRow = 0;
            foreach(MethodInfo m in t.GetMethods())
            {
                if(m.GetCustomAttribute<SensactCommandMethod>()!=null)
                {
                    if(functionTableRow==0)
                    {
                        commandsTable = new TableLayoutPanel
                        {
                            AutoSize = true,
                            //AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink,
                            //GrowStyle = System.Windows.Forms.TableLayoutPanelGrowStyle.AddRows,
                            //CellBorderStyle=TableLayoutPanelCellBorderStyle.OutsetDouble,
                        };
                    }
                    CreateControlsForOneCommandMethod(sac, m, commandsTable, functionTableRow);
                    functionTableRow++;
                }
            }
            if(commandsTable!=null)
            {
                this.masterTlp.Controls.Add(commandsTable, 0, row++);
            }
            else
            {
                masterTlp.Controls.Add(CreateLabel("No commands found"), 0, row++);
            }
            
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

        private void CreateControlsForOneCommandMethod(SensactApplicationContainer sac, MethodInfo mi, TableLayoutPanel functionTable, int functionTableRow)
        {
            int column = 0;
            Button btn = CreateCommandButton(sac, mi);
            functionTable.Controls.Add(btn, column++, functionTableRow);
            Config.CommandType ct = Config.CommandType.NOP;
            if(!Enum.TryParse<Config.CommandType>(mi.Name, out ct))
            {
                throw new Exception("Method name " + mi.Name + " that ist marked as SensactCommand cannot be parsed into a CommandType");
            }

            CommandSpecification cmdSpec = new CommandSpecification
            {
                applicationIdAsUshort = (ushort)sac.Index,
                CommandIdAsInt = (byte)ct,
            };
            
            foreach(ParameterInfo p in mi.GetParameters())
            {
                functionTable.Controls.Add(CreateLabel(p.Name), column++, functionTableRow);
                Control inp = null;
                if (IsDecimalType(p.ParameterType))
                {
                    inp = CreateDecimalParamInput(p);
                }
                else
                {
                    throw new NotSupportedException("Only decimal parameter types are supported");
                }
                functionTable.Controls.Add(inp, column++, functionTableRow);
                cmdSpec.C2Ps.Add(new Control2Parameter { TheControl = inp, TheParameter = p });
            }
            name2cmdSpec[btn.Name] = cmdSpec;
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

        private readonly Size BTN_SIZE = new Size(75, 23);
        private readonly Size LBL_SIZE = new Size(75, 23);

        private Button CreateCommandButton(SensactApplicationContainer sac, MethodInfo mi)
        {
            Button button=new Button();
            button.Size = BTN_SIZE;
            button.Text = mi.Name;
            button.Name = sac.Application.ApplicationId+"_"+mi.Name;
            button.UseVisualStyleBackColor = true;
            button.Click += new System.EventHandler(cmdButton_Click);
            return button;
        }

        private Label CreateLabel(string appName)
        {
            return new Label
            {
                AutoSize = false,
                Name = "lbl" + appName,
                Size = LBL_SIZE,
                Text = appName,
            };
        }

        private void cmdButton_Click(object sender, EventArgs e)
        {
            CommandSpecification cmdSpec = name2cmdSpec[(sender as Control).Name];
            byte[] buffer = new byte[100];
            byte pos = 1;
            buffer[pos] = 0x01; //0x01==START_OF_HEADING
            pos += 1;
            EndianBitConverter.Little.CopyBytes(cmdSpec.applicationIdAsUshort, buffer, pos);
            pos += 2;
            buffer[pos] = cmdSpec.CommandIdAsInt;
            pos += 1;
            foreach (var cmd in cmdSpec.C2Ps)
            {
                AddParam(buffer, cmd, ref pos);
            }
            buffer[0] = pos;
            serialPort.Write(buffer, 0, pos-1);
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

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }
    }
}
