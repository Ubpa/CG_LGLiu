using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace RS_GH_CS
{
    public class RS
    {
        public class API_CS
        {
            public int[] array;
            public int n;
        }

        public unsafe struct API_CPP
        {
            public int* _array;
            public int n;
        }

        [DllImport("RS_GH_CPP.dll")]

        private static unsafe extern bool RS_CPP(ref API_CPP api_cpp_in, out API_CPP api_cpp_out);

        public static bool RS_CS(API_CS api_cs_in, out API_CS api_cs_out)
        {
            api_cs_out = new API_CS();
            unsafe
            {
                API_CPP api_cpp_in = new API_CPP();
                API_CPP api_cpp_out = new API_CPP();
                fixed (int* array = &api_cs_in.array[0])
                {
                    api_cpp_in._array = array;
                    api_cpp_in.n = api_cs_in.n;
                    RS_CPP(ref api_cpp_in, out api_cpp_out);
                }

                api_cs_out.array = new int[api_cpp_out.n];
                for (int i = 0; i < api_cpp_out.n; i++)
                    api_cs_out.array[i] = api_cpp_out._array[i];
                api_cs_out.n = api_cpp_out.n;
            }//end unsafe
            
            return true;
        }
    }
}
