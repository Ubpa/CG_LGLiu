using System;
using System.Drawing;
using Grasshopper.Kernel;

namespace TF_INTRPL_CS
{
    public class TF_INTRPL_CSInfo : GH_AssemblyInfo
    {
        public override string Name
        {
            get
            {
                return "TFINTRPL";
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
                return new Guid("9fdba307-2820-452b-b27a-88312094cd6d");
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
