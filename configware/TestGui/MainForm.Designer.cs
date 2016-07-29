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
            this.components = new System.ComponentModel.Container();
            this.btnUpdateCOM = new System.Windows.Forms.Button();
            this.serialPort = new System.IO.Ports.SerialPort(this.components);
            this.cboSelectCOM = new System.Windows.Forms.ComboBox();
            this.btnOpenCOM = new System.Windows.Forms.Button();
            this.pnlMaster = new System.Windows.Forms.Panel();
            this.treeView1 = new System.Windows.Forms.TreeView();
            this.SuspendLayout();
            // 
            // btnUpdateCOM
            // 
            this.btnUpdateCOM.Location = new System.Drawing.Point(12, 12);
            this.btnUpdateCOM.Name = "btnUpdateCOM";
            this.btnUpdateCOM.Size = new System.Drawing.Size(92, 23);
            this.btnUpdateCOM.TabIndex = 0;
            this.btnUpdateCOM.Text = "Update COM";
            this.btnUpdateCOM.UseVisualStyleBackColor = true;
            this.btnUpdateCOM.Click += new System.EventHandler(this.btnUpdateCOM_Click);
            // 
            // serialPort
            // 
            this.serialPort.BaudRate = 115200;
            // 
            // cboSelectCOM
            // 
            this.cboSelectCOM.FormattingEnabled = true;
            this.cboSelectCOM.Location = new System.Drawing.Point(110, 14);
            this.cboSelectCOM.Name = "cboSelectCOM";
            this.cboSelectCOM.Size = new System.Drawing.Size(121, 21);
            this.cboSelectCOM.TabIndex = 1;
            // 
            // btnOpenCOM
            // 
            this.btnOpenCOM.Location = new System.Drawing.Point(237, 12);
            this.btnOpenCOM.Name = "btnOpenCOM";
            this.btnOpenCOM.Size = new System.Drawing.Size(75, 23);
            this.btnOpenCOM.TabIndex = 2;
            this.btnOpenCOM.Text = "Open Port";
            this.btnOpenCOM.UseVisualStyleBackColor = true;
            this.btnOpenCOM.Click += new System.EventHandler(this.btnOpenCOM_Click);
            // 
            // pnlMaster
            // 
            this.pnlMaster.Location = new System.Drawing.Point(299, 41);
            this.pnlMaster.Name = "pnlMaster";
            this.pnlMaster.Size = new System.Drawing.Size(673, 408);
            this.pnlMaster.TabIndex = 3;
            this.pnlMaster.BackColor = System.Drawing.Color.DarkMagenta;
            // 
            // treeView1
            // 
            this.treeView1.Location = new System.Drawing.Point(12, 41);
            this.treeView1.Name = "treeView1";
            this.treeView1.Size = new System.Drawing.Size(281, 408);
            this.treeView1.TabIndex = 0;
            this.treeView1.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.treeView1_AfterSelect);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(984, 461);
            this.Controls.Add(this.treeView1);
            this.Controls.Add(this.pnlMaster);
            this.Controls.Add(this.btnOpenCOM);
            this.Controls.Add(this.cboSelectCOM);
            this.Controls.Add(this.btnUpdateCOM);
            this.Name = "MainForm";
            this.Text = "Sensact Command Invoker";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button btnUpdateCOM;
        private System.IO.Ports.SerialPort serialPort;
        private System.Windows.Forms.ComboBox cboSelectCOM;
        private System.Windows.Forms.Button btnOpenCOM;
        private System.Windows.Forms.Control pnlMaster;
        private System.Windows.Forms.TreeView treeView1;
    }
}

