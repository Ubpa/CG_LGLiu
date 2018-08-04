using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

using Grasshopper.Kernel;
using Rhino.Geometry;

namespace Ubpa_GH
{
    public class API
    {
        public class WarpAPI_CS
        {
            public int[] R;
            public int[] B;
            public int[] G;

            public int width;
            public int height;

            public int[] px;
            public int[] py;

            public int[] fpx;
            public int[] fpy;

            public int n;

            public WarpAPI_CS()
            {
                R = G = B = null;
                width = height = 0;
                px = py = fpx = fpy = null;
                n = 0;
            }

            public void loadFrom(ref Mesh mesh, ref List<Point3d> p, ref List<Point3d> fp)
            {
                // step 1 : get width and height
                width = -1;
                for (int i = 0; i < mesh.Vertices.Count; i++)
                {
                    if (mesh.Vertices[i].X > width)
                        width = (int)mesh.Vertices[i].X;
                }
                height = mesh.Vertices.Count / width;

                // step 2 : get RGB
                R = new int[mesh.Vertices.Count];
                G = new int[mesh.Vertices.Count];
                B = new int[mesh.Vertices.Count];
                for (int i = 0; i < mesh.Vertices.Count; i++)
                {
                    R[i] = mesh.VertexColors[i].R;
                    G[i] = mesh.VertexColors[i].G;
                    B[i] = mesh.VertexColors[i].B;
                }

                // step 3 : get p , fp and n
                px = new int[p.Count];
                py = new int[p.Count];
                fpx = new int[p.Count];
                fpy = new int[p.Count];
                for (int i = 0; i < p.Count; i++)
                {
                    px[i] = (int)p[i].X;
                    py[i] = (int)p[i].Y;
                    fpx[i] = (int)fp[i].X;
                    fpy[i] = (int)fp[i].Y;
                }
                n = p.Count;
            }

            public void convertTo(ref Mesh mesh)
            {
                for (int i = 0; i < mesh.VertexColors.Count; i++)
                {
                    mesh.VertexColors.SetColor(i, R[i], G[i], B[i]);
                }
            }
        }

        public unsafe struct WarpAPI_CPP
        {
            public int* R;
            public int* B;
            public int* G;

            public int width;
            public int height;

            public int* px;
            public int* py;

            public int* fpx;
            public int* fpy;

            public int n;

            public void linkWith(ref WarpAPI_CS api_cs)
            {
                fixed (int* _R = api_cs.R, _G = api_cs.G, _B = api_cs.B,
                    _px = api_cs.px, _py = api_cs.py,
                    _fpx = api_cs.fpx, _fpy = api_cs.fpy)
                {
                    R = _R;
                    G = _G;
                    B = _B;
                    width = api_cs.width;
                    height = api_cs.height;
                    px = _px;
                    py = _py;
                    fpx = _fpx;
                    fpy = _fpy;
                    n = api_cs.n;
                }// end fixed
            }
        }

        [DllImport("CPP.dll")]
        private static unsafe extern bool WarpIDW_CPP(ref WarpAPI_CPP api_cpp);
        
        [DllImport("CPP.dll")]
        private static unsafe extern bool WarpRBF_CPP(ref WarpAPI_CPP api_cpp);

        [DllImport("CPP.dll")]
        private static unsafe extern bool MeshRect_CPP(ref WarpAPI_CPP api_cpp);
        
        [DllImport("CPP.dll")]
        private static unsafe extern bool MeshTriR_CPP(ref WarpAPI_CPP api_cpp);
        
        [DllImport("CPP.dll")]
        private static unsafe extern bool MeshTriF_CPP(ref WarpAPI_CPP api_cpp);

        public static bool WarpIDW_CS(ref WarpAPI_CS api_cs)
        {
            WarpAPI_CPP api_cpp = new WarpAPI_CPP();

            api_cpp.linkWith(ref api_cs);

            WarpIDW_CPP(ref api_cpp);
            return true;
        }
        
        public static bool WarpRBF_CS(ref WarpAPI_CS api_cs)
        {
            WarpAPI_CPP api_cpp = new WarpAPI_CPP();

            api_cpp.linkWith(ref api_cs);

            WarpRBF_CPP(ref api_cpp);
            return true;
        }

        public static bool MeshRect_CS(ref WarpAPI_CS api_cs)
        {
            WarpAPI_CPP api_cpp = new WarpAPI_CPP();

            api_cpp.linkWith(ref api_cs);

            MeshRect_CPP(ref api_cpp);
            return true;
        }

        public static bool MeshTriR_CS(ref WarpAPI_CS api_cs)
        {
            WarpAPI_CPP api_cpp = new WarpAPI_CPP();

            api_cpp.linkWith(ref api_cs);

            MeshTriR_CPP(ref api_cpp);
            return true;
        }
        
        public static bool MeshTriF_CS(ref WarpAPI_CS api_cs)
        {
            WarpAPI_CPP api_cpp = new WarpAPI_CPP();

            api_cpp.linkWith(ref api_cs);

            MeshTriF_CPP(ref api_cpp);
            return true;
        }
        
        //-------------------------------------------------------------------------------------------------------------------

        public class MeshAPI_CS
        {
            public int VerticesNumber;
            public int FaceNumber;
            public int TexCoordNumber;

            public float[] VerticesX;
            public float[] VerticesY;
            public float[] VerticesZ;

            public int[] FaceA;
            public int[] FaceB;
            public int[] FaceC;

            public float[] TexCoordX;
            public float[] TexCoordY;

            public MeshAPI_CS()
            {
                VerticesNumber = FaceNumber = TexCoordNumber = 0;
                VerticesX = VerticesY = VerticesZ = null;
                FaceA = FaceB = FaceC = null;
                TexCoordX = TexCoordY = null;
            }

            // only load V and F
            public void loadFrom(ref Mesh mesh)
            {
                VerticesNumber = mesh.Vertices.Count;
                VerticesX = new float[VerticesNumber];
                VerticesY = new float[VerticesNumber];
                VerticesZ = new float[VerticesNumber];
                for (int i = 0; i < VerticesNumber; i++)
                {
                    VerticesX[i] = mesh.Vertices[i].X;
                    VerticesY[i] = mesh.Vertices[i].Y;
                    VerticesZ[i] = mesh.Vertices[i].Z;
                }

                FaceNumber = mesh.Faces.Count;
                FaceA = new int[FaceNumber];
                FaceB = new int[FaceNumber];
                FaceC = new int[FaceNumber];
                for (int i = 0; i < FaceNumber; i++)
                {
                    FaceA[i] = mesh.Faces[i].A;
                    FaceB[i] = mesh.Faces[i].B;
                    FaceC[i] = mesh.Faces[i].C;
                }

                TexCoordNumber = 0;
                TexCoordX = null;
                TexCoordY = null;
            }
            // only load V
            public void loadFrom(ref Curve curve, int PointNum)
            {
                if (PointNum < 3)
                    return;

                VerticesNumber = PointNum;
                VerticesX = new float[PointNum];
                VerticesY = new float[PointNum];
                VerticesZ = new float[PointNum];
                
                Interval domain = curve.Domain;
                for (int i = 0; i < PointNum; i++)
                {
                    double t = ((double)i) / PointNum;
                    double param = (1 - t) * domain.Min + t * domain.Max;
                    Point3d point = curve.PointAt(param);
                    VerticesX[i] = (float)point.X;
                    VerticesY[i] = (float)point.Y;
                    VerticesZ[i] = (float)point.Z;
                }

                FaceNumber = 0;
                FaceA = null;
                FaceB = null;
                FaceC = null;

                TexCoordNumber = 0;
                TexCoordX = null;
                TexCoordY = null;
            }

            public void loadFrom(ref MeshAPI_CPP api_cpp)
            {
                unsafe
                {
                    VerticesNumber = api_cpp.VerticesNumber;
                    VerticesX = new float[VerticesNumber];
                    VerticesY = new float[VerticesNumber];
                    VerticesZ = new float[VerticesNumber];
                    for (int i = 0; i < VerticesNumber; i++)
                    {
                        VerticesX[i] = api_cpp.VerticesX[i];
                        VerticesY[i] = api_cpp.VerticesY[i];
                        VerticesZ[i] = api_cpp.VerticesZ[i];
                    }

                    FaceNumber = api_cpp.FaceNumber;
                    FaceA = new int[FaceNumber];
                    FaceB = new int[FaceNumber];
                    FaceC = new int[FaceNumber];
                    for (int i = 0; i < FaceNumber; i++)
                    {
                        FaceA[i] = api_cpp.FaceA[i];
                        FaceB[i] = api_cpp.FaceB[i];
                        FaceC[i] = api_cpp.FaceC[i];
                    }
                }// end unsafe
            }

            public void initTexCoord()
            {
                TexCoordNumber = VerticesNumber;
                TexCoordX = new float[TexCoordNumber];
                TexCoordY = new float[TexCoordNumber];
            }
            // only V and F
            public void convertTo(ref Mesh mesh)
            {
                mesh.Vertices.Clear();
                for (int i = 0; i < VerticesNumber; i++)
                {
                    float x = VerticesX[i];
                    float y = VerticesY[i];
                    float z = VerticesZ[i];
                    mesh.Vertices.Add(x, y, z);
                }

                mesh.Faces.Clear();
                for (int i = 0; i < FaceNumber; i++)
                {
                    int A = FaceA[i];
                    int B = FaceB[i];
                    int C = FaceC[i];
                    mesh.Faces.AddFace(A, B, C);
                }
            }

            public void updateTexCoordTo(ref Mesh mesh)
            {
                mesh.TextureCoordinates.Clear();
                
                float minX = TexCoordX[0];
                float maxX = TexCoordX[0];
                float minY = TexCoordY[0];
                float maxY = TexCoordY[0];
                for (int i = 0; i < TexCoordNumber; i++)
                {
                    minX = Math.Min(minX, TexCoordX[i]);
                    maxX = Math.Max(maxX, TexCoordX[i]);
                    minY = Math.Min(minY, TexCoordY[i]);
                    maxY = Math.Max(maxY, TexCoordY[i]);
                    mesh.TextureCoordinates.Add(TexCoordX[i], TexCoordY[i]);
                }
                float diffX = maxX - minX;
                float diffY = maxY - minY;
                for (int i = 0; i < TexCoordNumber; i++)
                {
                    float newX = (mesh.TextureCoordinates[i].X - minX)/diffX;
                    float newY = (mesh.TextureCoordinates[i].Y - minY)/diffY;
                    mesh.TextureCoordinates.SetTextureCoordinate(i, newX, newY);
                }
                /*
                for (int i = 0; i < TexCoordNumber; i++)
                {
                    double tcX = TexCoordX[i]-Math.Floor(TexCoordX[i]);
                    double tcY = TexCoordY[i]-Math.Floor(TexCoordY[i]);
                    mesh.TextureCoordinates.Add(tcX, tcY);
                }
                */
            }
        }

        public unsafe struct MeshAPI_CPP
        {
            public int VerticesNumber;
            public int FaceNumber;
            public int TexCoordNumber;

            public float* VerticesX;
            public float* VerticesY;
            public float* VerticesZ;

            public int* FaceA;
            public int* FaceB;
            public int* FaceC;

            public float* TexCoordX;
            public float* TexCoordY;

            public void linkWith(ref MeshAPI_CS api_cs)
            {
                fixed (float* _VerticesX = api_cs.VerticesX, _VerticesY = api_cs.VerticesY, _VerticesZ = api_cs.VerticesZ)
                {
                    VerticesNumber = api_cs.VerticesNumber;
                    VerticesX = _VerticesX;
                    VerticesY = _VerticesY;
                    VerticesZ = _VerticesZ;
                }
                fixed (int* _FaceA = api_cs.FaceA, _FaceB = api_cs.FaceB, _FaceC = api_cs.FaceC)
                {
                    FaceNumber = api_cs.FaceNumber;
                    FaceA = _FaceA;
                    FaceB = _FaceB;
                    FaceC = _FaceC;
                }
                fixed (float* _TexCoordX = api_cs.TexCoordX, _TexCoordY = api_cs.TexCoordY)
                {
                    TexCoordNumber = api_cs.TexCoordNumber;
                    TexCoordX = _TexCoordX;
                    TexCoordY = _TexCoordY;
                }
            }
        }
        
        [DllImport("CPP.dll")]
        private static unsafe extern bool MinSrfLocal_CPP(ref MeshAPI_CPP api_cpp);

        [DllImport("CPP.dll")]
        private static unsafe extern bool MinSrfGlobal_CPP(ref MeshAPI_CPP api_cpp);

        [DllImport("CPP.dll")]
        private static unsafe extern bool MinSrfCurve_CPP(ref MeshAPI_CPP api_cpp_in, out MeshAPI_CPP api_cpp_out);

        [DllImport("CPP.dll")]
        private static unsafe extern bool MeshAPIFree_CPP(ref MeshAPI_CPP api_cpp);

        [DllImport("CPP.dll")]
        private static unsafe extern bool Param_FB_U_CPP(ref MeshAPI_CPP api_cpp);
        
        [DllImport("CPP.dll")]
        private static unsafe extern bool Param_FB_WLS_CPP(ref MeshAPI_CPP api_cpp);
        
        [DllImport("CPP.dll")]
        private static unsafe extern bool Param_FB_SP_CPP(ref MeshAPI_CPP api_cpp);
        
        [DllImport("CPP.dll")]
        private static unsafe extern bool Param_UFB_ARAP_CPP(ref MeshAPI_CPP api_cpp);
        
        public static bool MinSrfLocal_CS(ref MeshAPI_CS api_cs)
        {
            MeshAPI_CPP api_cpp = new MeshAPI_CPP();
            api_cpp.linkWith(ref api_cs);
            MinSrfLocal_CPP(ref api_cpp);
            return true;
        }

        public static bool MinSrfGlobal_CS(ref MeshAPI_CS api_cs)
        {
            MeshAPI_CPP api_cpp = new MeshAPI_CPP();
            api_cpp.linkWith(ref api_cs);
            MinSrfGlobal_CPP(ref api_cpp);
            return true;
        }

        public static bool MinSrfCurve_CS(ref MeshAPI_CS api_cs, ref Mesh mesh)
        {
            MeshAPI_CPP api_cpp_in = new MeshAPI_CPP();
            api_cpp_in.linkWith(ref api_cs);
            MeshAPI_CPP api_cpp_out = new MeshAPI_CPP();
            MinSrfCurve_CPP(ref api_cpp_in, out api_cpp_out);
            api_cs.loadFrom(ref api_cpp_out);
            api_cs.convertTo(ref mesh);
            // free v and f
            MeshAPIFree_CPP(ref api_cpp_out);
            return true;
        }

        public static bool Param_FB_U_CS(ref MeshAPI_CS api_cs)
        {
            api_cs.initTexCoord();
            MeshAPI_CPP api_cpp = new MeshAPI_CPP();
            api_cpp.linkWith(ref api_cs);
            Param_FB_U_CPP(ref api_cpp);
            return true;
        }
        
        public static bool Param_FB_WLS_CS(ref MeshAPI_CS api_cs)
        {
            api_cs.initTexCoord();
            MeshAPI_CPP api_cpp = new MeshAPI_CPP();
            api_cpp.linkWith(ref api_cs);
            Param_FB_WLS_CPP(ref api_cpp);
            return true;
        }
        
        public static bool Param_FB_SP_CS(ref MeshAPI_CS api_cs)
        {
            api_cs.initTexCoord();
            MeshAPI_CPP api_cpp = new MeshAPI_CPP();
            api_cpp.linkWith(ref api_cs);
            Param_FB_SP_CPP(ref api_cpp);
            return true;
        }
        
        public static bool Param_UFB_ARAP_CS(ref MeshAPI_CS api_cs)
        {
            api_cs.initTexCoord();
            MeshAPI_CPP api_cpp = new MeshAPI_CPP();
            api_cpp.linkWith(ref api_cs);
            Param_UFB_ARAP_CPP(ref api_cpp);
            return true;
        }
    }
}
