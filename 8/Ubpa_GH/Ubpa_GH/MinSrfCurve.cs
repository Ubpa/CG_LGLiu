using System;
using System.Collections.Generic;

using Grasshopper.Kernel;
using Rhino.Geometry;

namespace Ubpa_GH
{
    public class MinSrfCurve : GH_Component
    {
        /// <summary>
        /// Initializes a new instance of the MinSrfCurve class.
        /// </summary>
        public MinSrfCurve()
            : base("MinSrfCurve", "MinSrfCurve",
                "MinSrfCurve",
                "Ubpa_GH", "Lab4")
        {
        }

        /// <summary>
        /// Registers all the input parameters for this component.
        /// </summary>
        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddCurveParameter("Curve", "C", "The curve to generate minimal surface mesh", GH_ParamAccess.item);
            pManager.AddIntegerParameter("PointNum", "PN", "The number on curve", GH_ParamAccess.item);
        }

        /// <summary>
        /// Registers all the output parameters for this component.
        /// </summary>
        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
            pManager.AddMeshParameter("Mesh", "M", "The minimal surface mesh of the curve", GH_ParamAccess.item);
        }

        /// <summary>
        /// This is the method that actually does the work.
        /// </summary>
        /// <param name="DA">The DA object is used to retrieve from inputs and store in outputs.</param>
        protected override void SolveInstance(IGH_DataAccess DA)
        {
            Curve curve = null;
            int pointNum = new int();
            if (!DA.GetData(0, ref curve))return;
            if (!DA.GetData(1, ref pointNum)) return;
            if (pointNum < 3)
            {
                base.AddRuntimeMessage(GH_RuntimeMessageLevel.Error, "PointNum is should be larger than 3.");
                return;
            }

            API.MeshAPI_CS api_cs = new API.MeshAPI_CS();
            api_cs.loadFrom(ref curve, pointNum);

            Mesh mesh = new Mesh();
            API.MinSrfCurve_CS(ref api_cs, ref mesh);

            //api_cs.convertTo(ref mesh);

            DA.SetData(0, mesh);
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
                return Ubpa_GH.images.minSurf_Curve;
            }
        }

        /// <summary>
        /// Gets the unique ID for this component. Do not change this ID after release.
        /// </summary>
        public override Guid ComponentGuid
        {
            get { return new Guid("581c6d21-3a38-4586-9b5e-6bb0c4738122"); }
        }
    }
}