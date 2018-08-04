using System;
using System.Collections.Generic;

using Grasshopper.Kernel;
using Rhino.Geometry;

namespace Ubpa_GH
{
    public class SimpleTexture : GH_Component
    {
        /// <summary>
        /// Initializes a new instance of the SimpleTexture class.
        /// </summary>
        public SimpleTexture()
            : base("SimpleTexture", "SimpleTexture",
                "SimpleTexture",
                "Ubpa_GH", "Lab5")
        {
        }

        /// <summary>
        /// Registers all the input parameters for this component.
        /// </summary>
        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddMeshParameter("Mesh", "M", "The mesh to paramaterize", GH_ParamAccess.item);
            pManager.AddMeshParameter("Image", "Img", "A mesh representation of the image", GH_ParamAccess.item);
        }

        /// <summary>
        /// Registers all the output parameters for this component.
        /// </summary>
        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
            pManager.AddMeshParameter("Mesh", "M", "Paramaterized Mesh", GH_ParamAccess.item);
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
            if (!DA.GetData(1, ref img)) return;

            int width = -1;
            for (int i = 0; i < img.Vertices.Count; i++)
            {
                if (img.Vertices[i].X > width)
                    width = (int)img.Vertices[i].X;
            }
            int height = img.Vertices.Count / width;

            int yMax = (height - 1);
            int xMax = (width - 1);
            mesh.VertexColors.Clear();
            for (int i = 0; i < mesh.Vertices.Count; i++)
            {
                int x = (int)Math.Round(xMax * mesh.TextureCoordinates[i].X, 1);
                int y = (int)Math.Round(yMax * mesh.TextureCoordinates[i].Y, 1);
                int idx = width * (yMax - y) + x;
                mesh.VertexColors.Add(img.VertexColors[idx]);
            }

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
                return Ubpa_GH.images.SimpleTexture;
            }
        }

        /// <summary>
        /// Gets the unique ID for this component. Do not change this ID after release.
        /// </summary>
        public override Guid ComponentGuid
        {
            get { return new Guid("7cbd682a-0c8f-4ae8-aff9-bc9b24f6df03"); }
        }
    }
}