using System;
using System.Collections.Generic;

using Grasshopper.Kernel;
using Rhino.Geometry;

namespace Ubpa_GH
{
    public class MinSrfGlobal : GH_Component
    {
        /// <summary>
        /// Initializes a new instance of the MinSrfGlobal class.
        /// </summary>
        public MinSrfGlobal()
            : base("MinSrfGlobal", "MinSrfGlobal",
                "MinSrfGlobal",
                "Ubpa_GH", "Lab4")
        {
        }

        /// <summary>
        /// Registers all the input parameters for this component.
        /// </summary>
        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddMeshParameter("Mesh", "M", "Mesh to Minimize", GH_ParamAccess.item);
        }

        /// <summary>
        /// Registers all the output parameters for this component.
        /// </summary>
        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
            pManager.AddMeshParameter("MinSrfMesh", "MSM", "Minimal Surface Mesh", GH_ParamAccess.item);
        }

        /// <summary>
        /// This is the method that actually does the work.
        /// </summary>
        /// <param name="DA">The DA object is used to retrieve from inputs and store in outputs.</param>
        protected override void SolveInstance(IGH_DataAccess DA)
        {
            Mesh mesh = new Mesh();
            if (!DA.GetData(0, ref mesh)) return;

            API.MeshAPI_CS api = new API.MeshAPI_CS();
            api.loadFrom(ref mesh);

            API.MinSrfGlobal_CS(ref api);

            Mesh rstM = new Mesh();
            rstM.CopyFrom(mesh);
            api.convertTo(ref rstM);

            DA.SetData(0, rstM);
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
                return Ubpa_GH.images.minSurf_Form;
            }
        }

        /// <summary>
        /// Gets the unique ID for this component. Do not change this ID after release.
        /// </summary>
        public override Guid ComponentGuid
        {
            get { return new Guid("083295ea-c5b1-400c-bc07-4f9617aa4c4b"); }
        }
    }
}