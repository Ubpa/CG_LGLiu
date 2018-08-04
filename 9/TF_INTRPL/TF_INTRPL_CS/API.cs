using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

using Grasshopper.Kernel;
using Rhino.Geometry;

namespace TF_INTRPL_CS
{
    class API
    {
        public class API_CS
        {
            public double[] px;
            public double[] py;
            public double[] pz;

            public double[] Apx;
            public double[] Apy;
            public double[] Apz;

            public int num;
            public double t;

            public double[] transform;

            public API_CS()
            {
                px = py = pz = null;
                Apx = Apy = Apz = null;
                num = 0;
                t = 0.0;
                transform = null;
            }

            public bool loadFrom(ref Mesh startMesh, ref Mesh endMesh, double _t)
            {
                if (_t < 0.0 || _t > 1.0)
                    return false;
                if (startMesh.Vertices.Count != endMesh.Vertices.Count)
                    return false;
                if (startMesh.Vertices.Count < 3)
                    return false;

                t = _t;
                num = startMesh.Vertices.Count;
                px = new double[num];
                py = new double[num];
                pz = new double[num];
                Apx = new double[num];
                Apy = new double[num];
                Apz = new double[num];

                for (int i = 0; i < num; i++)
                {
                    px[i] = startMesh.Vertices[i].X;
                    py[i] = startMesh.Vertices[i].Y;
                    pz[i] = startMesh.Vertices[i].Z;
                    Apx[i] = endMesh.Vertices[i].X;
                    Apy[i] = endMesh.Vertices[i].Y;
                    Apz[i] = endMesh.Vertices[i].Z;
                }

                transform = new double[16];

                return true;
            }

            public void convertTo(ref Mesh mesh, ref Transform tf)
            {
                mesh.Vertices.Clear();
                for (int i = 0; i < num; i++)
                {
                    mesh.Vertices.Add(Apx[i], Apy[i], Apz[i]);
                }

                tf.M00 = transform[0];
                tf.M01 = transform[1];
                tf.M02 = transform[2];
                tf.M03 = transform[3];
                tf.M10 = transform[4];
                tf.M11 = transform[5];
                tf.M12 = transform[6];
                tf.M13 = transform[7];
                tf.M20 = transform[8];
                tf.M21 = transform[9];
                tf.M22 = transform[10];
                tf.M23 = transform[11];
                tf.M30 = transform[12];
                tf.M31 = transform[13];
                tf.M32 = transform[14];
                tf.M33 = transform[15];
            }
        }

        public unsafe struct API_CPP
        {
            public double* px;
            public double* py;
            public double* pz;

            public double* Apx;
            public double* Apy;
            public double* Apz;

            public int num;
            public double t;

            public double* transform;

            public void linkWith(ref API_CS api_cs)
            {
                num = api_cs.num;
                t = api_cs.t;
                fixed (double* _px = api_cs.px, _py = api_cs.py, _pz = api_cs.pz,
                    _Apx = api_cs.Apx, _Apy = api_cs.Apy, _Apz = api_cs.Apz,
                    _transform = api_cs.transform)
                {
                    px = _px;
                    py = _py;
                    pz = _pz;
                    Apx = _Apx;
                    Apy = _Apy;
                    Apz = _Apz;
                    transform = _transform;
                }// end fixed
            }
        }

        [DllImport("TF_INTRPL_CPP.dll")]
        private static unsafe extern bool TF_Interpolation_CPP(ref API_CPP api_cpp);

        public static bool TF_INTRPL_CS(ref API_CS api_cs)
        {
            API_CPP api_cpp = new API_CPP();

            api_cpp.linkWith(ref api_cs);

            return TF_Interpolation_CPP(ref api_cpp);
        }
    }
}
