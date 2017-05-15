namespace Klli.Sensact.TestGui
{
    partial class MainForm
    {
        /// <summary>
        /// Erforderliche Designervariable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Verwendete Ressourcen bereinigen.
        /// </summary>
        /// <param name="disposing">True, wenn verwaltete Ressourcen gelöscht werden sollen; andernfalls False.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Vom Windows Form-Designer generierter Code

        /// <summary>
        /// Erforderliche Methode für die Designerunterstützung.
        /// Der Inhalt der Methode darf nicht mit dem Code-Editor geändert werden.
        /// </summary>
        private void InitializeComponent()
        {
            this.btnUpdateCOM = new System.Windows.Forms.Button();
            this.cboSelectCOM = new System.Windows.Forms.ComboBox();
            this.btnOpenCOM = new System.Windows.Forms.Button();
            this.cntrlMaster = new System.Windows.Forms.Panel();
            this.treeView1 = new System.Windows.Forms.TreeView();
            this.lisTerminal = new System.Windows.Forms.ListBox();
            this.btnSetTime = new System.Windows.Forms.Button();
            this.btnGetTime = new System.Windows.Forms.Button();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.btnSelectFile = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // btnUpdateCOM
            // 
            this.btnUpdateCOM.Location = new System.Drawing.Point(16, 15);
            this.btnUpdateCOM.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.btnUpdateCOM.Name = "btnUpdateCOM";
            this.btnUpdateCOM.Size = new System.Drawing.Size(123, 28);
            this.btnUpdateCOM.TabIndex = 0;
            this.btnUpdateCOM.Text = "Update COM";
            this.btnUpdateCOM.UseVisualStyleBackColor = true;
            this.btnUpdateCOM.Click += new System.EventHandler(this.btnUpdateCOM_Click);
            // 
            // cboSelectCOM
            // 
            this.cboSelectCOM.FormattingEnabled = true;
            this.cboSelectCOM.Location = new System.Drawing.Point(147, 17);
            this.cboSelectCOM.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.cboSelectCOM.Name = "cboSelectCOM";
            this.cboSelectCOM.Size = new System.Drawing.Size(160, 24);
            this.cboSelectCOM.TabIndex = 1;
            // 
            // btnOpenCOM
            // 
            this.btnOpenCOM.Location = new System.Drawing.Point(316, 15);
            this.btnOpenCOM.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.btnOpenCOM.Name = "btnOpenCOM";
            this.btnOpenCOM.Size = new System.Drawing.Size(100, 28);
            this.btnOpenCOM.TabIndex = 2;
            this.btnOpenCOM.Text = "Open Port";
            this.btnOpenCOM.UseVisualStyleBackColor = true;
            this.btnOpenCOM.Click += new System.EventHandler(this.btnOpenCOM_Click);
            // 
            // cntrlMaster
            // 
            this.cntrlMaster.BackColor = System.Drawing.Color.DarkMagenta;
            this.cntrlMaster.Location = new System.Drawing.Point(399, 50);
            this.cntrlMaster.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.cntrlMaster.Name = "cntrlMaster";
            this.cntrlMaster.Size = new System.Drawing.Size(897, 502);
            this.cntrlMaster.TabIndex = 3;
            // 
            // treeView1
            // 
            this.treeView1.Location = new System.Drawing.Point(16, 50);
            this.treeView1.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.treeView1.Name = "treeView1";
            this.treeView1.Size = new System.Drawing.Size(373, 501);
            this.treeView1.TabIndex = 0;
            this.treeView1.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.treeView1_AfterSelect);
            // 
            // lisTerminal
            // 
            this.lisTerminal.Font = new System.Drawing.Font("Consolas", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lisTerminal.FormattingEnabled = true;
            this.lisTerminal.ItemHeight = 17;
            this.lisTerminal.Location = new System.Drawing.Point(16, 560);
            this.lisTerminal.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.lisTerminal.Name = "lisTerminal";
            this.lisTerminal.ScrollAlwaysVisible = true;
            this.lisTerminal.Size = new System.Drawing.Size(1279, 208);
            this.lisTerminal.TabIndex = 4;
            // 
            // btnSetTime
            // 
            this.btnSetTime.Location = new System.Drawing.Point(424, 15);
            this.btnSetTime.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.btnSetTime.Name = "btnSetTime";
            this.btnSetTime.Size = new System.Drawing.Size(100, 28);
            this.btnSetTime.TabIndex = 5;
            this.btnSetTime.Text = "Set Time";
            this.btnSetTime.UseVisualStyleBackColor = true;
            this.btnSetTime.Click += new System.EventHandler(this.btnSetTime_Click);
            // 
            // btnGetTime
            // 
            this.btnGetTime.Location = new System.Drawing.Point(532, 15);
            this.btnGetTime.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.btnGetTime.Name = "btnGetTime";
            this.btnGetTime.Size = new System.Drawing.Size(100, 28);
            this.btnGetTime.TabIndex = 6;
            this.btnGetTime.Text = "GetTime";
            this.btnGetTime.UseVisualStyleBackColor = true;
            this.btnGetTime.Click += new System.EventHandler(this.btnGetTime_Click);
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileName = "openFileDialog";
            // 
            // btnSelectFile
            // 
            this.btnSelectFile.Location = new System.Drawing.Point(1195, 13);
            this.btnSelectFile.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.btnSelectFile.Name = "btnSelectFile";
            this.btnSelectFile.Size = new System.Drawing.Size(100, 28);
            this.btnSelectFile.TabIndex = 7;
            this.btnSelectFile.Text = "Select File";
            this.btnSelectFile.UseVisualStyleBackColor = true;
            this.btnSelectFile.Click += new System.EventHandler(this.btnSelectFile_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1312, 789);
            this.Controls.Add(this.btnSelectFile);
            this.Controls.Add(this.btnGetTime);
            this.Controls.Add(this.btnSetTime);
            this.Controls.Add(this.lisTerminal);
            this.Controls.Add(this.treeView1);
            this.Controls.Add(this.cntrlMaster);
            this.Controls.Add(this.btnOpenCOM);
            this.Controls.Add(this.cboSelectCOM);
            this.Controls.Add(this.btnUpdateCOM);
            this.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.Name = "MainForm";
            this.Text = "Sensact Command Invoker";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button btnUpdateCOM;
        private System.Windows.Forms.ComboBox cboSelectCOM;
        private System.Windows.Forms.Button btnOpenCOM;
        private System.Windows.Forms.TreeView treeView1;
        private System.Windows.Forms.ListBox lisTerminal;
        private System.Windows.Forms.Button btnSetTime;
        private System.Windows.Forms.Button btnGetTime;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.Button btnSelectFile;
        private System.Windows.Forms.Panel cntrlMaster;
    }
}

