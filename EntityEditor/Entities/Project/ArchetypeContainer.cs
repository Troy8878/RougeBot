﻿using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace EntityEditor.Entities.Project
{
    public class ArchetypeContainer : ITreeOwner
    {
        public ArchetypeContainer()
            : this(Path.Combine(
                MainWindow.Instance.RepoDir, 
                "Roguelike", "Assets", "Entities"))
        {
            Name = "Archetypes";
        }

        public ArchetypeContainer(string dir)
        {
            Directories = new List<ArchetypeContainer>();
            Archetypes = new List<Archetype>();

            var dirinfo = new DirectoryInfo(dir);
            Name = dirinfo.Name;

            foreach (var subdir in dirinfo.GetDirectories())
            {
                Directories.Add(new ArchetypeContainer(subdir.FullName));
            }

            foreach (var file in dirinfo.GetFiles())
            {
                Archetypes.Add(new Archetype(file));
            }
        }

        public List<ArchetypeContainer> Directories { get; set; } 
        public List<Archetype> Archetypes { get; set; }

        public string Name { get; set; }

        public string Type
        {
            get { return "Archetypes"; }
        }

        public IEnumerable<object> OwnedItems
        {
            // ReSharper disable once PossibleMultipleEnumeration
            get { return new IEnumerable<object>[] {Directories, Archetypes}.SelectMany(l => l); }
        }
    }
}