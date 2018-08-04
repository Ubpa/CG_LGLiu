using System;
using System.Collections.Generic;

using Grasshopper.Kernel;
using Rhino.Geometry;

namespace Ubpa_GH
{
    public class WarpTriF : GH_Component
    {
        /// <summary>
        /// Initializes a new instance of the WarpTriF class.
        /// </summary>
        public WarpTriF()
            : base("WarpTriF", "WarpTriF",
                "WarpTriF",
                "Ubpa_GH", "Lab3")
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
        /// <param name="DA">The DA object is used to retrieve from inputs and store in outputs.</param>
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

            API.MeshTriF_CS(ref api_cs);

            Mesh rst_mesh = new Mesh();
            rst_mesh.CopyFrom(mesh);
            api_cs.convertTo(ref rst_mesh);

            DA.SetData(0, rst_mesh);
        }

        /// <summary>
        /// Provides an Icon for the component.
        /// </summary>
        protected override System.Drawing.Bitmap Icon
        {
            get
            {
                //You can add image files to your project resources and access them like this:
                // return Resources.IconForThisComponent;
                return Ubpa_GH.images.tri_feature_mesh;
            }
        }

        /// <summary>
        /// Gets the unique ID for this component. Do not change this ID after release.
        /// </summary>
        public override Guid ComponentGuid
        {
            get { return new Guid("ab76cfbf-9320-45a1-9900-fefaf2d4909a"); }
        }
    }
}