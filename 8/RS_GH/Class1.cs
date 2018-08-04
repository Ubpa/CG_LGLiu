using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace RS_GH_CS
{
    public class RS
    {
        public class Input_CS
        {
            public int[] array;
            public int n;
        }

        public class Output_CS
        {
            public int[] r_array;
            public int n;
            public int sum;
        }

        public unsafe class Input_CPP
        {
            public int* array;
            public int n;
        }

        public unsafe class Output_CPP
        {
            public int* r_array;
            public int n;
            public int sum;
        }

        [DllImport("RS_GH_CPP.dll")]

        private static unsafe extern bool RS_CPP(ref Input_CPP input_cpp, out Output_CPP output_cpp);

        public static bool RS_CS(Input_CS input_cs, out Output_CS output_cs)
        {
            output_cs = new Output_CS();
            unsafe
            {
                Input_CPP input_cpp = new Input_CPP();
                Output_CPP output_cpp = new Output_CPP();
                fixed (int* array = &input_cs.array[0])
                {
                    input_cpp.array = array;
                    input_cpp.n = input_cs.n;
                    RS_CPP(ref input_cpp, out output_cpp);
                }

                output_cs.r_array = new int[output_cpp.n];
                for (int i = 0; i < output_cpp.n; i++)
                    output_cs.r_array[i] = output_cpp.r_array[i];
                output_cs.n = output_cpp.n;
                output_cs.sum = output_cpp.sum;
            }//end unsafe
            
            return true;
        }
    }
}
