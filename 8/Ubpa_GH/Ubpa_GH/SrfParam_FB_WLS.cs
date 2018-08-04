using System;
using System.Collections.Generic;

using Grasshopper.Kernel;
using Rhino.Geometry;

namespace Ubpa_GH
{
    public class SrfParam_FB_WLS : GH_Component
    {
        /// <summary>
        /// Initializes a new instance of the SrfParam_FB_WSL class.
        /// </summary>
        public SrfParam_FB_WLS()
            : base("SrfParam_FB_WLS", "SrfParam_FB_WLS",
                "SrfParam_FB_WLS",
                "Ubpa_GH", "Lab5")
        {
        }

        /// <summary>
        /// Registers all the input parameters for this component.
        /// </summary>
        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddMeshParameter("Mesh", "M", "The mesh to paramaterize", GH_ParamAccess.item);
        }

        /// <summary>
        /// Registers all the output parameters for this component.
        /// </summary>
        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
            pManager.AddMeshParameter("Mesh", "M", "Paramaterized Mesh", GH_ParamAccess.item);
            pManager.AddNumberParameter("TexCoord_X", "TC_X", "Texture Coordinates X", GH_ParamAccess.list);
            pManager.AddNumberParameter("TexCoord_Y", "TC_Y", "Texture Coordinates Y", GH_ParamAccess.list);
        }

        /// <summary>
        /// This is the method that actually does the work.
        /// </summary>
        /// <param name="DA">The DA object is used to retrieve from inputs and store in outputs.</param>
        protected override void SolveInstance(IGH_DataAccess DA)
        {
            Mesh mesh = new Mesh();
            Mesh img = new Mesh();
            if (!DA.GetData(0, ref mesh)) return;

            API.MeshAPI_CS api_cs = new API.MeshAPI_CS();
            api_cs.loadFrom(ref mesh);
            API.Param_FB_WLS_CS(ref api_cs);

            Mesh rstMesh = new Mesh();
            rstMesh.CopyFrom(mesh);
            api_cs.updateTexCoordTo(ref rstMesh);

            List<float> texCoord_X = new List<float>();
            List<float> texCoord_Y = new List<float>();
            for (int i = 0; i < rstMesh.Vertices.Count; i++)
            {
                texCoord_X.Add(rstMesh.TextureCoordinates[i].X);
                texCoord_Y.Add(rstMesh.TextureCoordinates[i].Y);
            }

            DA.SetData(0, rstMesh);
            DA.SetDataList(1, texCoord_X);
            DA.SetDataList(2, texCoord_Y);
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
                return Ubpa_GH.images.tex_WLS;
            }
        }

        /// <summary>
        /// Gets the unique ID for this component. Do not change this ID after release.
        /// </summary>
        public override Guid ComponentGuid
        {
            get { return new Guid("aadde5c5-6c63-4f59-8158-27719c63e25b"); }
        }
    }
}