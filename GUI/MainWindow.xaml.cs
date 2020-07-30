using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using WinForm = System.Windows.Forms;
using Microsoft.VisualBasic.Devices;

using System.IO;

namespace GUI
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {

        static public FtpWebRequest ftp = null;
        static public String root = "/";
        static public int timeout = 300000;
        IWebProxy proxy = null;

        public String IP_ADDRESS 
        {
            get { return txt_IP.Text; }
            set { txt_IP.Text = value; }
        }
        public int PORT
        {
            get 
            {
                try { return int.Parse(txt_Port.Text); }
                catch (Exception) { txt_Connection_Info.Text += "\n端口号非数字";return -1; }
            }
            set { txt_Port.Text = value.ToString(); }
        }
        public String ACCOUNT
        {
            get { return txt_Account.Text; }
            set { txt_Account.Text = value; }
        }
        public String PASSWORD
        {
            get { return txt_Password.Password; }
            set { txt_Password.Password = value; }
        }
        public String LOCAL_PATH
        {
            get { return txt_Local_Dic.Text; }
            set { txt_Local_Dic.Text = value; }
        }
        String remote_path = root;
        public String REMOTE_PATH
        {
            get 
            {
                return new Uri(new Uri("ftp://" + IP_ADDRESS.TrimEnd('/')), remote_path).ToString();
            }
            set 
            {
                string result = root;
                if (!string.IsNullOrEmpty(value) && value != root)
                {
                    result = Path.Combine(Path.Combine(root, value.TrimStart('/').TrimEnd('/')), "/"); // 进行路径的拼接
                }
                remote_path = result;
                txt_Remote_Dic.Text = new Uri(new Uri("ftp://" + IP_ADDRESS.TrimEnd('/')), remote_path).ToString(); ;
            }
        }
        public String LOCAL_PICKED
        {
            get
            {
                if (lst_Local.SelectedItem != null)
                {
                    return lst_Local.SelectedItem.ToString().Substring(4);
                }
                else
                {
                    txt_File_Info.Text += "\n文件不存在！";
                    return "";
                }
            }
        }
        public String REMOTE_PICKED
        {
            get
            {
                if (lst_Remote.SelectedItem != null)
                {
                    if (lst_Remote.SelectedItem.ToString() != "/..")
                        return lst_Remote.SelectedItem.ToString().Substring(4);
                    else
                        return "..";
                }
                else
                {
                    txt_File_Info.Text += "\n文件不存在！";
                    return "";
                }
            }
        }
        public String UPLOAD_FROM
        {
            get
            {
                if (lst_Local.SelectedItem != null)
                {
                   return LOCAL_PATH.TrimStart('\\') + "\\" + LOCAL_PICKED;
                }
                else
                {
                    return "";
                }
            }
        }
        public String UPLOAD_TO
        {
            get
            {
                if (!String.IsNullOrEmpty(REMOTE_PATH))
                {
                    return REMOTE_PATH.TrimStart('/') + "/" + LOCAL_PICKED;
                }
                else
                {
                    return "";
                }
            }
        }
        public String DOWNLOAD_FROM
        {
            get
            {
                if (lst_Remote.SelectedItem != null)
                {
                    return REMOTE_PATH.TrimStart('/') + "/" + REMOTE_PICKED;
                }
                else
                {
                    return "";
                }
            }
        }
        public String DOWNLOAD_TO
        {
            get
            {
                if (!String.IsNullOrEmpty(LOCAL_PATH))
                {
                    return LOCAL_PATH.TrimStart('\\') + "\\" + REMOTE_PICKED;
                }
                else
                {
                    return "";
                }
            }
        }
        public bool ENABLE_SSL
        {
            get;
            set;
        }
        public bool USE_PASSIVE
        {
            get;
            set;
        }
        public bool USE_BINARY
        {
            get;
            set;
        }

        public bool connected = false;

        public MainWindow()
        {
            InitializeComponent();
        }
        private void Local_Refresh()
        {
            lst_Local.Items.Clear();
            //目录
            DirectoryInfo dir = new DirectoryInfo(LOCAL_PATH);
            foreach (DirectoryInfo d in dir.GetDirectories())
            {
                lst_Local.Items.Add("[目录]" + d.Name);
            }
            //文件
            FileInfo[] file = dir.GetFiles("*.*");
            foreach (FileInfo f in file)
            {
                lst_Local.Items.Add("[文件]" + f.Name);
            }
        }
        private void Remote_Refresh()
        {
            lst_Remote.Items.Clear();
            lst_Remote.Items.Add("/..");
            //目录
            DirectoryInfo dir = new DirectoryInfo("C:\\Users\\92887\\Desktop\\Test For FTP Server");
            foreach (DirectoryInfo d in dir.GetDirectories())
            {
                lst_Remote.Items.Add("[目录]" + d.Name);
            }
            //文件
            FileInfo[] file = dir.GetFiles("*.*");
            foreach (FileInfo f in file)
            {
                lst_Remote.Items.Add("[文件]" + f.Name);
            }
        }

        private string URLCombine(string ip_address, string remote_path, string file_name)
        {
            string result = new Uri(new Uri(new Uri("ftp://"+ip_address.TrimEnd('/')), remote_path), file_name).ToString(); 
            return result;
        }
        private FtpWebRequest Create_Connection(string url, string method)
        {
            FtpWebRequest request = (FtpWebRequest)FtpWebRequest.Create(new Uri(url));
            request.Credentials = new NetworkCredential(ACCOUNT, PASSWORD);
            request.Proxy = proxy;
            request.KeepAlive = false;
            request.UseBinary = USE_BINARY;
            request.UsePassive = USE_PASSIVE;
            request.EnableSsl = ENABLE_SSL;
            request.Method = method;
            Console.WriteLine(request);
            return request;
        }
        private void btn_Reset_Click(object sender, RoutedEventArgs e)
        {
            IP_ADDRESS = "";
            PORT = 0;
            ACCOUNT = "";
            PASSWORD = "";
        }
        private void btn_Default_Click(object sender, RoutedEventArgs e)
        {
            IP_ADDRESS = "192.168.0.100";
            PORT = 21;
            ACCOUNT = "test";
            PASSWORD = "test";
        }
        private void btn_Connect_Click(object sender, RoutedEventArgs e)
        {
            if(ftp == null)
            {
                
                if (!connected)
                {
                    try
                    {
                        String url = URLCombine(IP_ADDRESS, remote_path, "");
                        ftp = (FtpWebRequest)FtpWebRequest.Create(new Uri(url));
                        ftp.Credentials = new NetworkCredential(ACCOUNT, PASSWORD);
                        connected = true;
                        txt_Connection_Info.Text += "\n连接成功！";
                        btn_Connect.Content = "断开";
                        REMOTE_PATH = "/";
                        Remote_Refresh();
                    }
                    catch (Exception ex)
                    {
                        txt_Connection_Info.Text += "\n无法连接！";
                        txt_Connection_Info.Text += ex.Message;
                    }
                }
                else
                {
                    try
                    {
                        ftp.Abort();
                        connected = false;
                        txt_Connection_Info.Text += "\n连接中断！";
                        btn_Connect.Content = "连接";
                    }
                    catch(Exception)
                    {
                        txt_Connection_Info.Text += "\n终止连接失败！";
                    }
                }
                    
                
            }
        }
        private void btn_Local_Dic_Click(object sender, RoutedEventArgs e)
        {
            WinForm.FolderBrowserDialog dialog = new WinForm.FolderBrowserDialog();
           
            if(dialog.ShowDialog() == WinForm.DialogResult.OK)
            {
                LOCAL_PATH = dialog.SelectedPath;
            }
            if (LOCAL_PATH.Trim().Length == 0)//判断文件名不为空
            {
                return;
            }
            try
            {
                Local_Refresh();
            }
            catch (Exception ex)
            {
                txt_File_Info.Text += "\n" + ex.ToString();//防止有些文件无权限访问，屏蔽异常
            }
            
        }
        private void btn_Local_Refresh_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                Local_Refresh();
            }
            catch (Exception ex)
            {
                txt_File_Info.Text += "\n" + ex.ToString();//防止有些文件无权限访问，屏蔽异常
            }
        }
        private void btn_Local_Rename_Click(object sender, RoutedEventArgs e)
        {
            if (lst_Local.SelectedItem != null)
            {
                try
                {
                    Name_Dialog rename = new Name_Dialog();
                    rename.txt_name.Text = LOCAL_PICKED;
                    rename.ShowDialog();
                    if (rename.will_name)
                    {
                        Computer MyComputer = new Computer();
                        if (Directory.Exists(UPLOAD_FROM))
                        {
                            MyComputer.FileSystem.RenameDirectory(UPLOAD_FROM, rename.txt_name.Text);
                        }
                        else if (File.Exists(UPLOAD_FROM))
                        {
                            MyComputer.FileSystem.RenameFile(UPLOAD_FROM, rename.txt_name.Text);
                        }
                        else
                        {
                            throw (new Exception("文件/文件夹不存在！"));
                        }
                        
                    }
                }
                catch(Exception ex)
                {
                    txt_File_Info.Text += "\n" + ex.ToString();
                }
            }
        }
        private void btn_Local_Newfolder_Click(object sender, RoutedEventArgs e)
        {
            if (Directory.Exists(LOCAL_PATH))
            {
                try
                {
                    Name_Dialog name = new Name_Dialog();
                    name.txt_name.Text = "";
                    name.ShowDialog();
                    if (name.will_name)
                    {
                        String dic_name = LOCAL_PATH + "\\" + name.txt_name.Text;
                        if (!Directory.Exists(dic_name))//如果不存在就创建 dir 文件夹  
                            Directory.CreateDirectory(dic_name);
                    }
                }
                catch (Exception ex)
                {
                    txt_File_Info.Text += "\n" + ex.ToString();
                }
            }
            
        }
        private void btn_Local_Delete_Click(object sender, RoutedEventArgs e)
        {
            if (lst_Local.SelectedItem != null)
            {
                try
                {
                    if (Directory.Exists(UPLOAD_FROM))
                    {
                        Directory.Delete(UPLOAD_FROM, true);
                    }
                    else if (File.Exists(UPLOAD_FROM))
                    {
                        File.Delete(UPLOAD_FROM);
                    }
                    else
                    {
                        throw (new Exception("文件/文件夹不存在！"));
                    }
                }
                catch (Exception ex)
                {
                    txt_File_Info.Text += "\n" + ex.ToString();
                }
            }
        }
        private void btn_Upload_Click(object sender, RoutedEventArgs e)
        {
            if (!connected) { txt_Connection_Info.Text += "\n未连接！";return; }
            if (lst_Local.SelectedItem == null) { txt_File_Info.Text += "\n未选择文件";return; }
            if (Directory.Exists(UPLOAD_FROM)) { txt_File_Info.Text += "\n不支持文件夹！";return; }
            if (!File.Exists(UPLOAD_FROM)) { txt_File_Info.Text += "\n文件不存在！";return; }
            try
            {
                String url = URLCombine(IP_ADDRESS, REMOTE_PATH, LOCAL_PICKED);
                FtpWebRequest request = Create_Connection(url, WebRequestMethods.Ftp.UploadFile);
                using (Stream rs = request.GetRequestStream())
                {
                    FileInfo fileInfo = new FileInfo(UPLOAD_FROM);
                    using (FileStream fs = fileInfo.OpenRead())
                    {
                        byte[] buffer = new byte[1024 * 4];
                        int count = fs.Read(buffer, 0, buffer.Length);
                        while (count > 0)
                        {
                            rs.Write(buffer, 0, count);
                            count = fs.Read(buffer, 0, buffer.Length);
                        }
                        fs.Close();
                    }
                }
            }
            catch (Exception ex)
            {
                txt_File_Info.Text += "\n" + ex.ToString();
            }
        }
        private void btn_Remote_Dic_Click(object sender, RoutedEventArgs e)
        {
            if (!connected) { txt_Connection_Info.Text += "\n未连接！"; return; }
            if (lst_Remote.SelectedItem != null)
            {
                try
                {
                    if(DOWNLOAD_FROM.EndsWith(".."))
                    {
                        String current = DOWNLOAD_FROM.Substring(0, DOWNLOAD_FROM.LastIndexOf("/"));
                        String upper = current.Substring(0, current.LastIndexOf("/"));
                        REMOTE_PATH = upper;
                    }
                    else if (Directory.Exists(DOWNLOAD_FROM))
                    {
                        REMOTE_PATH = DOWNLOAD_FROM;
                        Remote_Refresh();
                    }
                }
                catch (Exception ex)
                {
                    txt_File_Info.Text += "\n" + ex.ToString();
                }
            }
        }
        private void btn_Remote_Refresh_Click(object sender, RoutedEventArgs e)
        {
            if (!connected) { txt_Connection_Info.Text += "\n未连接！"; return; }
            try
            {
                Remote_Refresh();
            }
            catch (Exception ex)
            {
                txt_File_Info.Text += "\n" + ex.ToString();//防止有些文件无权限访问，屏蔽异常
            }
        }
        private void btn_Remote_Rename_Click(object sender, RoutedEventArgs e)
        {
            throw new NotImplementedException();
        }
        private void btn_Remote_Newfolder_Click(object sender, RoutedEventArgs e)
        {
            throw new NotImplementedException();
        }
        private void btn_Remote_Delete_Click(object sender, RoutedEventArgs e)
        {
            throw new NotImplementedException();
        }
        private void btn_Download_Click(object sender, RoutedEventArgs e)
        {
            if (!connected) { txt_Connection_Info.Text += "\n未连接！"; return; }
            if (lst_Remote.SelectedItem == null) { txt_File_Info.Text += "\n未选择文件"; return; }
            if (Directory.Exists(DOWNLOAD_FROM)) { txt_File_Info.Text += "\n不支持文件夹！"; return; }
            if (!File.Exists(DOWNLOAD_FROM)) { txt_File_Info.Text += "\n文件不存在！"; return; }
            using (FileStream fs = new FileStream(DOWNLOAD_TO, FileMode.OpenOrCreate))
            {
                try
                {
                    string url = URLCombine(IP_ADDRESS, REMOTE_PATH, REMOTE_PICKED);
                    Console.WriteLine(url);

                    FtpWebRequest request = Create_Connection(url, WebRequestMethods.Ftp.DownloadFile);
                    request.ContentOffset = fs.Length;
                    using (FtpWebResponse response = (FtpWebResponse)request.GetResponse())
                    {
                        fs.Position = fs.Length;
                        byte[] buffer = new byte[1024 * 4];
                        int count = response.GetResponseStream().Read(buffer, 0, buffer.Length);
                        while (count > 0)
                        {
                            fs.Write(buffer, 0, count);
                            count = response.GetResponseStream().Read(buffer, 0, buffer.Length);
                        }
                        response.GetResponseStream().Close();
                    }
                }
                catch (WebException ex)
                {
                    txt_File_Info.Text += "\n" + ex.ToString();
                }
            }
        }
    }
}
