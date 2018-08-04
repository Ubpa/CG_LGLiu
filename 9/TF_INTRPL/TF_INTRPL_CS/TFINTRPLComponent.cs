using System;
using System.Collections.Generic;

using Grasshopper.Kernel;
using Rhino.Geometry;

namespace TF_INTRPL_CS
{
    public class TFINTRPLComponent : GH_Component
    {
        /// <summary>
        /// Each implementation of GH_Component must provide a public 
        /// constructor without any arguments.
        /// Category represents the Tab in which the component will appear, 
        /// Subcategory the panel. If you use non-existing tab or panel names, 
        /// new tabs/panels will automatically be created.
        /// </summary>
        public TFINTRPLComponent()
            : base("TF_INTRPL", "TF_INTRPL",
                "Transform Interpolation",
                "Transform", "Euclidean")
        {
        }

        /// <summary>
        /// Registers all the input parameters for this component.
        /// </summary>
        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddMeshParameter("StartMesh", "M0", "The Start Mesh", GH_ParamAccess.item);
            pManager.AddMeshParameter("EndMesh", "M1", "The End Mesh", GH_ParamAccess.item);
            pManager.AddNumberParameter("t", "t", "0 < t < 1", GH_ParamAccess.item);
        }

        /// <summary>
        /// Registers all the output parameters for this component.
        /// </summary>
        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
            pManager.AddMeshParameter("ResultMesh", "Mt", "The Result Mesh", GH_ParamAccess.item);
            pManager.AddTransformParameter("Transform", "T", "The Transform", GH_ParamAccess.item);
        }

        /// <summary>
        /// This is the method that actually does the work.
        /// </summary>
        /// <param name="DA">The DA object can be used to retrieve data from input parameters and 
        /// to store data in output parameters.</param>
        protected override void SolveInstance(IGH_DataAccess DA)
        {
            Mesh M0 = new Mesh();
            Mesh M1 = new Mesh();
            double t = 0.0;
            if (!DA.GetData(0, ref M0)) return;
            if (!DA.GetData(1, ref M1)) return;
            if (!DA.GetData(2, ref t)) return;
            if (t < 0.0 || t > 1.0)
            {
                base.AddRuntimeMessage(GH_RuntimeMessageLevel.Error, "0 <= t <= 1");
                return;
            }

            API.API_CS api_cs = new API.API_CS();
            //base.AddRuntimeMessage(GH_RuntimeMessageLevel.Error, "flag1");
            if (!api_cs.loadFrom(ref M0, ref M1, t))
            {
                base.AddRuntimeMessage(GH_RuntimeMessageLevel.Error, "M0 and M1 is not same");
                return;
            }
            //base.AddRuntimeMessage(GH_RuntimeMessageLevel.Error, "flag2");
            
            if (!API.TF_INTRPL_CS(ref api_cs))
            {
                base.AddRuntimeMessage(GH_RuntimeMessageLevel.Error, "API.TF_INTRPL_CS(ref api_cs) error");
                return;
            }
            //base.AddRuntimeMessage(GH_RuntimeMessageLevel.Error, "flag3");

            Mesh Mt = new Mesh();
            Mt.Faces.AddFaces(M0.Faces);
            Transform tf = new Transform();
            api_cs.convertTo(ref Mt, ref tf);
            Mt.FaceNormals.Clear();
            Mt.FaceNormals.ComputeFaceNormals();
            Mt.UnifyNormals();
            //base.AddRuntimeMessage(GH_RuntimeMessageLevel.Error, "flag4");

            DA.SetData(0, Mt);
            DA.SetData(1, tf);
        }

        /// <summary>
        /// Provides an Icon for every component that will be visible in the User Interface.
        /// Icons need to be 24x24 pixels.
        /// </summary>
        protected override System.Drawing.Bitmap Icon
        {
            get
            {
                // You can add image files to your project resources and access them like this:
                //return Resources.IconForThisComponent;
                return TF_INTRPL_CS.images.icon;
            }
        }

        /// <summary>
        /// Each component must have a unique Guid to identify it. 
        /// It is vital this Guid doesn't change otherwise old ghx files 
        /// that use the old ID will partially fail during loading.
        /// </summary>
        public override Guid ComponentGuid
        {
            get { return new Guid("f5831e63-118a-4bcb-9824-c45fd8569d49"); }
        }
    }
}
