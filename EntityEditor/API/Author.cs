using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;
using EntityEditor.Properties;
using LibGit2Sharp;

namespace EntityEditor.API
{
    public class Author
    {
        private string _user;
        private string _pass;

        public Author(string name, string email, string user, string pass)
        {
            IsComplete = false;
            Name = name;
            Email = email;
            _user = user;
            _pass = pass;
        }

        public static Author Load()
        {
            var author = Construct();

            try
            {
                if (string.IsNullOrWhiteSpace(author.User))
                    author.IsComplete = false;
                if (string.IsNullOrWhiteSpace(author.Pass))
                    author.IsComplete = false;
            }
            catch
            {
                author.IsComplete = false;
                author.User = "";
                author.Pass = "";
            }

            return author;
        }

        private static Author Construct()
        {
            var settings = Settings.Default;
            var author = new Author(
                settings.AuthorName,
                settings.AuthorEmail,
                settings.AuthorGitUser,
                settings.AuthorGitPass);
            author.CheckCompleteness();
            return author;
        }

        public bool CheckCompleteness()
        {
            IsComplete = true;
            try
            {
                if (string.IsNullOrWhiteSpace(User))
                {
                    return IsComplete = false;
                }
                if (string.IsNullOrWhiteSpace(Pass))
                {
                    return IsComplete = false;
                }
            }
            catch
            {
                if (!IsComplete)
                    User = "";
                Pass = "";

                return IsComplete = false;
            }

            if (string.IsNullOrWhiteSpace(Name))
                return IsComplete = false;

            if (string.IsNullOrWhiteSpace(Email))
                return IsComplete = false;

            return true;
        }

        public void Save()
        {
            if (!CheckCompleteness())
                throw new Exception("Fields are not complete");

            var settings = Settings.Default;
            settings.AuthorName = Name;
            settings.AuthorEmail = Email;
            settings.AuthorGitUser = _user;
            settings.AuthorGitPass = _pass;
            settings.Save();
        }

        public bool IsComplete { get; private set; }

        public string Name { get; set; }

        public string Email { get; set; }

        public string User
        {
            get { return Decrypt(_user); }
            set { _user = Encrypt(value); }
        }

        public string Pass
        {
            private get { return Decrypt(_pass); }
            set { _pass = Encrypt(value); }
        }

        public Signature GetSignature()
        {
            return new Signature(Name, Email, DateTime.Now);
        }

        public Credentials Credentials
        {
            get
            {
                return new UsernamePasswordCredentials
                {
                    Username = Decrypt(_user),
                    Password = Decrypt(_pass)
                };
            }
        }

        private static string Encrypt(string data)
        {
            var bytes = Encoding.UTF8.GetBytes(data);
            var encrypted = ProtectedData.Protect(bytes, EncryptEntropy, DataProtectionScope.CurrentUser);
            return Convert.ToBase64String(encrypted);
        }

        private static string Decrypt(string data)
        {
            var bytes = Convert.FromBase64String(data);
            var decrypted = ProtectedData.Unprotect(bytes, EncryptEntropy, DataProtectionScope.CurrentUser);
            return Encoding.UTF8.GetString(decrypted);
        }

        private static readonly byte[] EncryptEntropy = Encoding.UTF8.GetBytes(@"
            Pony ipsum dolor sit amet pumpkin Cake dolore ex ullamco Mayor Mare hot
            air balloon sed deserunt flank kindness Equestria. Cloud Gilda cupcake
            Bloomberg. And that's how Equestria was made! Aute Braeburn nisi aliquip
            Zecora id minim. Vinyl Scratch Steven Magnet magna Granny Smith qui do
            aute et. Velit cloud Mayor Mare, tail proident Vinyl Scratch Pinkie Pie
            laboris alicorn laugher Pumpkin Cake.

            Caramel do Scootaloo muffin, Sapphire Shores Equestria Cranky Doodle
            Donkey dolore consequat Ponyville Twilight Sparkle. Ea irure Filthy Rich
            Spitfire minim anim. Cranky Doodle Donkey Pinkie Pie eu Silver Spoon
            Bloomberg Philomena labore cupcakes. Discord Big McIntosh veniam
            Lightning Dust dragon Smarty Pants. Veniam Trixie Cranky Doodle Donkey
            nostrud Ms. Peachbottom cillum.

            Bloomberg ut Ms. Harshwhinny magic muffin cloud pies Angel consequat Mr.
            Cake enim nulla Lightning Dust. Ex Babs Seed duis deserunt pies wing
            Cranky Doodle Donkey exercitation Gummy alicorn, Donut Joe nisi officia
            anim. Mollit Nightmare Moon kindness, Dr Hooves Trixie Gummy Braeburn
            sunt Snips party moon Daring Do. Ex tail culpa rainbow Hoity Toity
            Equestria ad Ms. Harshwhinny.

            Pegasai party Ms. Harshwhinny, Princess Celestia sed Caramel Lyra
            kindness Angel Babs Seed King Sombra Cheerilee ut Trixie. Pariatur
            consequat Caramel, pies anim Owlowiscious Opalescence duis Ms.
            Peachbottom proident elements of harmony apples Sweetie Belle Flash
            Sentry. Sweetie Belle Flam pegasai Babs Seed. Sunset Shimmer Babs Seed
            magic Vinyl Scratch cupcakes. Everfree Forest aute ut et pegasai Dr Hooves
            Ms. Peachbottom velit unicorns tail Shadowbolts unicorn qui horn.

            It needs to be about 20% cooler. Sed honesty sint chaos consequat cupcakes.
            Canterlot Opalescence dolor rainbow power, Pound Cake in aute incididunt in
            fugiat Smarty Pants Filthy Rich tail King Sombra apples. Filthy Rich Flash
            Sentry voluptate Iron Will velit.
        ");
    }
}
