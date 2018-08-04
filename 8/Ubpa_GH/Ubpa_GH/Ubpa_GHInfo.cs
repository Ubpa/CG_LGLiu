using System;
using System.Drawing;
using Grasshopper.Kernel;

namespace Ubpa_GH
{
    public class Ubpa_GHInfo : GH_AssemblyInfo
    {
        public override string Name
        {
            get
            {
                return "UbpaGH";
            }
        }
        public override Bitmap Icon
        {
            get
            {
                //Return a 24x24 pixel bitmap to represent this GHA library.
                return null;
            }
        }
        public override string Description
        {
            get
            {
                //Return a short string describing the purpose of this GHA library.
                return "";
            }
        }
        public override Guid Id
        {
            get
            {
                return new Guid("afb932d4-2e60-4daa-b1c2-6c983e64b2da");
            }
        }

        public override string AuthorName
        {
            get
            {
                //Return a string identifying you or your company.
                return "";
            }
        }
        public override string AuthorContact
        {
            get
            {
                //Return a string representing your preferred contact details.
                return "";
            }
        }
    }
}
