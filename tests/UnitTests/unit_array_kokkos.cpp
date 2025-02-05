/****************************************************************************
* Copyright (c) 2024, CEA
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/

#pragma GCC diagnostic push

#if __GNUC__ < 9
#pragma GCC diagnostic ignored "-Wsuggest-override"
#endif

#include <gtest/gtest.h>

#pragma GCC diagnostic pop

#include <TRUSTArray.h>

#ifdef __NVCOMPILER
#pragma diag_suppress 177
#endif


//Reads a single value from a device view to compare it on the host
double read_device_value(DoubleArrView& view, const int i){
    double value;
    Kokkos::deep_copy(value, Kokkos::subview(view, i));
    return value;
}

//Const version
double read_device_value(CDoubleArrView& view, const int i){
    double value;
    Kokkos::deep_copy(value, Kokkos::subview(view, i));
    return value;
}

TEST(TRUSTArrayKokkos, KokkosAccessorRO) {
    {
        int N=100;
        ArrOfDouble array(100);
        for (int i = 0; i < N; ++i) {array[i]=i;}

        //This does the H2D sync !
        auto view_ro = array.view_ro();

        //Can I access ? Is the value right ?
        for (int i = 0; i < N; ++i) {EXPECT_EQ(array[i], read_device_value(view_ro, i));}
    }
}

TEST(TRUSTArrayKokkos, KokkosAccessorWO) {
    {
        int N=100;
        ArrOfDouble array(100);

        auto view_wo = array.view_wo();

        //Can I write ?
        Kokkos::parallel_for(N, KOKKOS_LAMBDA(const int i) {
        view_wo(i) = 2*i;
        });        
        
        //Is the array correctly modified ?
        for (int i = 0; i < N; ++i) {EXPECT_EQ(array[i], 2*i);}

    }
}

TEST(TRUSTArrayKokkos, KokkosAccessorRW) {
    {
        int N=100;
        ArrOfDouble array(100);
        for (int i = 0; i < N; ++i) {array[i]=i;}
        
        //This does H2D
        auto view_rw = array.view_rw();

        //Can I read / write ?
        Kokkos::parallel_for("DoubleElements", N, KOKKOS_LAMBDA(const int i) {
        view_rw(i) = 2 * view_rw(i);
        });        

        //Is the array correctly modified and transferred back to the host ?
        for (int i = 0; i < N; ++i) {EXPECT_EQ(array[i], 2*i);}

    }
}

TEST(TRUSTArrayKokkos, KokkosAccessorSync) {
    {
        int N=100;
        ArrOfDouble array(100);
        for (int i = 0; i < N; ++i) {array[i] = 0;}

        auto view_rw = array.view_rw();

        Kokkos::parallel_for("Kernel 1", N, KOKKOS_LAMBDA(int i) {view_rw(i) += 1; });
        Kokkos::fence();
        Kokkos::parallel_for("Kernel 2", N, KOKKOS_LAMBDA(int i) {view_rw(i) += 1; });

        for (int i = 0; i < N; ++i) {EXPECT_EQ(read_device_value(view_rw, i), 2);}
    }
}

//Tests the common mistake to add a cpu kernel between two GPU kernel
//The accessor [] makes an implicit D2H copy... Unclear how and if it should be
//converted to kokkos too..
TEST(TRUSTArrayKokkos, KokkosAccessorSyncError) {
    {
        int N=100;
        ArrOfDouble array(100);

        for (int i = 0; i < N; ++i) {array[i]=0;}
        //CPU: 0, GPU: Non allocated

        auto view_rw = array.view_rw();
        // CPU: 0, GPU: 0

        Kokkos::parallel_for("Kernel 1", N, KOKKOS_LAMBDA(int i) {view_rw(i) += 1; });
        // CPU: 0, GPU: 1

        Kokkos::fence();

        for (int i = 0; i < N; ++i) {array[i] += 1;} //implicit D2H copy
        // CPU: 2, GPU: 1

        Kokkos::parallel_for("Kernel 2", N, KOKKOS_LAMBDA(int i) {view_rw(i) += 1; });
        // CPU: 2, GPU: 2

#ifdef TRUST_USE_GPU //sur GPU ?
        for (int i = 0; i < N; ++i) {EXPECT_EQ(read_device_value(view_rw, i), 2);}
        for (int i = 0; i < N; ++i) {EXPECT_EQ(array[i], 2);}
#else
        for (int i = 0; i < N; ++i) {EXPECT_EQ(read_device_value(view_rw, i), 3);}
#endif
    }
}

//A view_rw should do the trick
TEST(TRUSTArrayKokkos, KokkosAccessorSyncErrorFixed) {
    {
        int N=100;
        ArrOfDouble array(100);
        for (int i = 0; i < N; ++i) {array[i]=0;}
        //CPU: 0, GPU: Non allocated

        auto view_rw = array.view_rw();
        //CPU: 0, GPU: 0

        Kokkos::parallel_for("Kernel 1", N, KOKKOS_LAMBDA(int i) {view_rw(i) += 1; });
        //CPU: 0, GPU: 1

        Kokkos::fence();

        for (int i = 0; i < N; ++i) {array[i] += 1;}
        //CPU: 2, GPU: 1

        view_rw = array.view_rw(); //mark device as modified
        //CPU: 2, GPU: 2

        Kokkos::parallel_for("Kernel 2", N, KOKKOS_LAMBDA(int i) {view_rw(i) += 1; });
        //CPU: 2, GPU: 3

#ifdef TRUST_USE_GPU //sur GPU ?
        for (int i = 0; i < N; ++i) {EXPECT_EQ(read_device_value(view_rw, i), 3);}
        for (int i = 0; i < N; ++i) {EXPECT_EQ(array[i], 3);}
        //CPU: 3, GPU: 3 the accessor made a copy because device was marked as modified!
#else
        for (int i = 0; i < N; ++i) {EXPECT_EQ(read_device_value(view_rw, i), 3);}
#endif
    }
}

// TEST(TRUSTArrayKokkos, FlattenedTabAccessors) {

//         int n0=10, n1=11, n2=12;

//         TRUSTTab<double, int> tab(n0,n1,n2);

//         TRUSTArray<double, int> &tab_array = tab;

//         TRUSTArray<double, int> array(n0);

//         //Correct dimensions, especially when casted into an array
//         EXPECT_EQ(tab.nb_dim(), 3);
//         EXPECT_EQ(tab_array.nb_dim(), 3);

//         //Same size in memory
//         EXPECT_EQ(tab_array.size_array(), n0*n1*n2);
//         EXPECT_EQ(tab.size_array(), n0*n1*n2);

        //You cannot use the wrong SHAPE on a tab 4!=3
        //EXPECT_DEATH( {tab.check_flattened<4>(); }, ".*" );

        //You cannot use a SHAPE>1 on an array;
        //EXPECT_DEATH( {array.check_flattened<4>(); }, ".*" );
        //EXPECT_DEATH( {tab_array.check_flattened<4>(); }, ".*" );

//         //Creation of a flattened view on tab
//         auto view_tab_1 = tab.view_rw<1, Kokkos::DefaultExecutionSpace>();
//         auto view_tab_array_1 = tab_array.view_rw<1, Kokkos::DefaultExecutionSpace>();

//         auto view_tab_3 = tab.view_rw<3, Kokkos::DefaultExecutionSpace>();

//         //Correct extent
//         EXPECT_EQ((int)view_tab_1.extent(0), n0*n1*n2);
//         EXPECT_EQ((int)view_tab_array_1.extent(0), n0*n1*n2);

//         EXPECT_EQ((int)view_tab_3.extent(0), n0);
//         EXPECT_EQ((int)view_tab_3.extent(1), n1);
//         EXPECT_EQ((int)view_tab_3.extent(2), n2);

//         //Compatible with _SHAPE_=1, detects flattening
//         EXPECT_TRUE(tab.check_flattened<1>());
//         EXPECT_TRUE(tab_array.check_flattened<1>());
//         EXPECT_TRUE(array.check_flattened<1>());

//         //OK to create a multiD view on a 1D tab
//         TRUSTTab<double, int> tab_1D(n0);
//         EXPECT_FALSE(tab_1D.check_flattened<4>());
// }

#ifdef __NVCOMPILER
#pragma diag_default 177
#endif

