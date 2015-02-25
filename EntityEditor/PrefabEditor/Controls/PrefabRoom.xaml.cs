using System.IO;
using EntityEditor.PrefabEditor.Prefabs;
using Microsoft.Win32;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace EntityEditor.PrefabEditor.Controls
{
    /// <summary>
    ///     Interaction logic for PrefabRoom.xaml
    /// </summary>
    public partial class PrefabRoom
    {
        private string _file;

        private readonly OpenFileDialog _openDlg = new OpenFileDialog
        {
            Filter = "Prefab JSON File|*.prefab.json"
        };

        private readonly SaveFileDialog _saveDlg = new SaveFileDialog
        {
            Filter = "Prefab JSON File|*.prefab.json"
        };

        public PrefabRoom()
        {
            InitializeComponent();

            SetEmpty();
        }

        public Stream OpenFileWrite(string name)
        {
            return File.Open(name, FileMode.Create, FileAccess.Write);
        }

        public Stream OpenFileRead(string name)
        {
            return File.Open(name, FileMode.Open, FileAccess.Read);
        }

        public void SetEmpty()
        {
            _file = null;

            var data = new PrefabTileData[10][];
            for (var i = 0; i < 10; ++i)
            {
                data[i] = new PrefabTileData[10];
                for (var j = 0; j < 10; ++j)
                {
                    data[i][j] = (PrefabTileData) Tiles.Prefabs[0];
                }
            }
            Items.ItemsSource = data;
        }

        public void SetFile(string name)
        {
            _file = name;
            _openDlg.FileName = name;
            _saveDlg.FileName = name;
        }

        public bool HasFile()
        {
            return _file != null;
        }

        public void Load()
        {
            if (_openDlg.ShowDialog() != true)
            {
                return;
            }

            using (var file = OpenFileRead(_openDlg.FileName))
            {
                Load(file);
            }

            _file = _openDlg.FileName;
            _saveDlg.FileName = _openDlg.FileName;
        }

        public void Load(Stream file)
        {
            EditHistory.ClearHistory();

            using (var reader = new StreamReader(file))
            {
                var tiles = (PrefabTileData[][]) Items.ItemsSource;

                var data = JObject.Parse(reader.ReadToEnd());
                var jtiles = (JArray) data["tiles"];
                for (var y = 0; y < 10; ++y)
                {
                    var jrow = (JArray) jtiles[y];
                    for (var x = 0; x < 10; ++x)
                    {
                        var jtile = (JObject) jrow[x];
                        var tile = tiles[y][x];
                        tile.FromSerialized(jtile);
                    }
                }
            }
        }

        public void Save()
        {
            if (_file != null)
            {
                using (var file = OpenFileWrite(_file))
                {
                    Save(file);
                }
                return;
            }

            if (_saveDlg.ShowDialog() != true)
            {
                return;
            }

            using (var file = OpenFileWrite(_saveDlg.FileName))
            {
                Save(file);
            }
            
            SetFile(_saveDlg.FileName);

            Lib.Refresh(null, null);
        }

        public void Save(Stream file)
        {
            using (var writer = new StreamWriter(file))
            {
                var data = Serialize().ToString(Formatting.Indented);
                writer.Write(data);
            }
        }

        public JObject Serialize()
        {
            var root = new JObject();

            var tiles = new JArray();
            foreach (var row in (PrefabTileData[][])Items.ItemsSource)
            {
                var jrow = new JArray();
                foreach (var tile in row)
                {
                    jrow.Add(tile.Serialize());
                }
                tiles.Add(jrow);
            }
            root["tiles"] = tiles;

            return root;
        }
    }
}