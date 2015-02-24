using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using EntityEditor.Prefabs;
using Microsoft.Win32;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace EntityEditor.Controls
{
    /// <summary>
    ///     Interaction logic for PrefabRoom.xaml
    /// </summary>
    public partial class PrefabRoom
    {
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

        public void SetEmpty()
        {
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

        public void Load()
        {
            if (_openDlg.ShowDialog() != true)
            {
                return;
            }

            using (var file = _openDlg.OpenFile())
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
                        tile.Reassign((PrefabTileData) Tiles.Prefabs[(int) jtile["id"]]);

                        var meta = jtile["meta"];
                        if (meta != null)
                            tile.Metadata = (string) meta;
                    }
                }
            }

            _saveDlg.FileName = _openDlg.FileName;
        }

        public void Save()
        {
            if (_saveDlg.ShowDialog() != true)
            {
                return;
            }

            using (var file = _saveDlg.OpenFile())
            using (var writer = new StreamWriter(file))
            {
                var data = Serialize().ToString(Formatting.Indented);
                writer.Write(data);
            }

            _openDlg.FileName = _saveDlg.FileName;
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
                    var jtile = new JObject();
                    jtile["id"] = tile.Base.Id;
                    jtile["meta"] = tile.Metadata;
                    jrow.Add(jtile);
                }
                tiles.Add(jrow);
            }
            root["tiles"] = tiles;

            return root;
        }
    }
}