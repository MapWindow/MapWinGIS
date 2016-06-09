namespace MWLite.Databases
{
    public class ConnectionParams
    {
        public string Host { get; set;}
        public int Port { get; set; }
        public string Database { get; set;}
        public string UserName { get; set;}
        public string Password { get; set; }

        public void InitDefaultPostGis()
        {
            Database = "";
            Password = "";
            Port = 5432;
            UserName = "postgres";
            Host = "127.0.0.1";
        }

        public string GetPostGisConnection()
        {
            string cs = "PG:host={0} port={1} dbname={2} user={3} password={4}";
            return string.Format(cs, Host, Port, Database, UserName, Password);
        }
    }
}
