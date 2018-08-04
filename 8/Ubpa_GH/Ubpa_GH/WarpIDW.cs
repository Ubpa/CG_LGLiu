using System;
using System.Collections.Generic;

using Grasshopper.Kernel;
using Rhino.Geometry;

namespace Ubpa_GH
{   
    public class WarpIDW : GH_Component
    {
        /// <summary>
        /// Each implementation of GH_Component must provide a public 
        /// constructor without any arguments.
        /// Category represents the Tab in which the component will appear, 
        /// Subcategory the panel. If you use non-existing tab or panel names, 
        /// new tabs/panels will automatically be created.
        /// </summary>
        public WarpIDW()
            : base("WarpIDW", "WarpIDW",
                "WarpIDW",
                "Ubpa_GH", "Lab2")
        {
        }

        /// <summary>
        /// Registers all the input parameters for this component.
        /// </summary>
        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddMeshParameter("Image", "Img", "A mesh representation of the image", GH_ParamAccess.item);
            pManager.AddPointParameter("p", "p", "The list of start points", GH_ParamAccess.list);
            pManager.AddPointParameter("fp", "fp", "The list of changes", GH_ParamAccess.list);
        }

        /// <summary>
        /// Registers all the output parameters for this component.
        /// </summary>
        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
            pManager.AddMeshParameter("Image", "Img", "A mesh representation of the image", GH_ParamAccess.item);
        }

        /// <summary>
        /// This is the method that actually does the work.
        /// </summary>
        /// <param name="DA">The DA object can be used to retrieve data from input parameters and 
        /// to store data in output parameters.</param>
        protected override void SolveInstance(IGH_DataAccess DA)
        {
            Mesh mesh = new Mesh();
            List<Point3d> p = new List<Point3d>();
            List<Point3d> fp = new List<Point3d>();
            if (!DA.GetData(0, ref mesh)) return;
            if (!DA.GetDataList(1, p)) return;
            if (!DA.GetDataList(2, fp)) return;
            if (p.Count != fp.Count) return;

            API.WarpAPI_CS api_cs = new API.WarpAPI_CS();

            api_cs.loadFrom(ref mesh, ref p, ref fp);

            API.WarpIDW_CS(ref api_cs);

            Mesh rst_mesh = new Mesh();
            rst_mesh.CopyFrom(mesh);
            api_cs.convertTo(ref rst_mesh);

            DA.SetData(0, rst_mesh);
        }

        /// <summary>
        /// Provides a icon
        /// for every component that will be visible in the User Interface.
        /// Icons need to be 24x24 pixels.
        /// </summary>
        protected override System.Drawing.Bitmap Icon
        {
            get
            {
                // You can add image files to your project resources and access them like this:
                //return Resources.IconForThisComponent;
                return Ubpa_GH.images.idw;
            }
        }

        /// <summary>
        /// Each component must have a unique Guid to identify it. 
        /// It is vital this Guid doesn't change otherwise old ghx files 
        /// that use the old ID will partially fail during loading.
        /// </summary>
        public override Guid ComponentGuid
        {
            get { return new Guid("6a03e685-f81a-4892-b9fb-91e8586ce581"); }
        }
    }
}
