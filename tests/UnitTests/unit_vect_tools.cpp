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

#include <TRUSTTab_parts.h>

//=== This is testing src/Kernel/Math/TRUSTVect_tools.cpp

// Templated test function to factor testing for both double and float instantiations
template <typename _TYPE_>
void TestAjouteProduitScalaire() {
    const int size = 5;
    TRUSTVect<_TYPE_, int> z(size), x(size), y(size);
    _TYPE_ alpha = _TYPE_(2.0);

    // Initialize vectors x and y, and set initial values for z
    for (int i = 0; i < size; i++) {
        x(i) = _TYPE_(i + 1);  // x = [1, 2, 3, 4, 5]
        y(i) = _TYPE_(i + 1);  // y = [1, 2, 3, 4, 5]
        z(i) = _TYPE_(-i);     // z = [0, -1, -2, -3, -4]
    }

    // Call ajoute_produit_scalaire
    ajoute_produit_scalaire<_TYPE_, int>(z, alpha, x, y, Mp_vect_options::VECT_ALL_ITEMS);

    // Calculate and check expected result for each element in z
    for (int i = 0; i < size; i++) {
        _TYPE_ expected_value = alpha * x(i) * y(i) - _TYPE_(i);
        EXPECT_EQ(z(i), expected_value) << "Mismatch at index " << i << " for type " << typeid(_TYPE_).name();
    }
}

// Wrapper tests for both double and float types
TEST(TRUSTVectTools, AjouteProduitScalaireDouble) {
    TestAjouteProduitScalaire<double>();
}
TEST(TRUSTVectTools, AjouteProduitScalaireFloat) {
    TestAjouteProduitScalaire<float>();
}


// Templated test function to factor testing for both MUL and DIV operations and both float and double types
template <typename _TYPE_, TYPE_OPERATION_VECT_SPEC_GENERIC _TYPE_OP_>
void TestOperationSpecialeTresGeneric() {
    const int size = 5;
    TRUSTVect<_TYPE_, int> resu(size), resu_(size), vx(size);
    Mp_vect_options opt = Mp_vect_options::VECT_ALL_ITEMS;

    // Initialize vx with values and set resu to a starting value
    for (int i = 0; i < size; i++) {
        vx(i) = _TYPE_(i + 10);
        resu(i) = _TYPE_(1);
        resu_(i)=resu(i);
    }

    // Call operation_speciale_tres_generic
    operation_speciale_tres_generic<_TYPE_OP_, _TYPE_, int>(resu, vx, opt);

    // Calculate expected results based on the operation
    for (int i = 0; i < size; i++) {
        _TYPE_ expected_value;
        if (_TYPE_OP_ == TYPE_OPERATION_VECT_SPEC_GENERIC::MUL_) {
            expected_value = resu_(i) * vx(i);  // For MUL, resu *= vx
        } else { // DIV operation
            expected_value = resu_(i) / vx(i);  // For DIV, resu /= vx
        }
        EXPECT_EQ(resu(i), expected_value) << "Mismatch at index " << i << " for type " << typeid(_TYPE_).name();
    }
}

// Wrapper tests for each type and operation
TEST(TRUSTVectTools, OperationSpecialeTresGenericMulDouble) {
    TestOperationSpecialeTresGeneric<double, TYPE_OPERATION_VECT_SPEC_GENERIC::MUL_>();
}

TEST(TRUSTVectTools, OperationSpecialeTresGenericMulFloat) {
    TestOperationSpecialeTresGeneric<float, TYPE_OPERATION_VECT_SPEC_GENERIC::MUL_>();
}

TEST(TRUSTVectTools, OperationSpecialeTresGenericDivDouble) {
    TestOperationSpecialeTresGeneric<double, TYPE_OPERATION_VECT_SPEC_GENERIC::DIV_>();
}

TEST(TRUSTVectTools, OperationSpecialeTresGenericDivFloat) {
    TestOperationSpecialeTresGeneric<float, TYPE_OPERATION_VECT_SPEC_GENERIC::DIV_>();
}

// Templated test function for ADD and SQUARE operations with float and double types
template <typename _TYPE_, TYPE_OPERATION_VECT_SPEC _TYPE_OP_>
void TestAjouteOperationSpecialeGeneric() {
    const int size = 5;
    TRUSTVect<_TYPE_, int> resu(size), resu_(size), vx(size);
    _TYPE_ alpha = _TYPE_(2.0);
    Mp_vect_options opt = Mp_vect_options::VECT_ALL_ITEMS;

    // Initialize `vx` with values and `resu` to 0
    for (int i = 0; i < size; i++) {
        vx(i) = _TYPE_(i + 1);
        resu(i) = _TYPE_(-10*i);
        resu_(i) = resu(i);
    }

    // Call ajoute_operation_speciale_generic
    ajoute_operation_speciale_generic<_TYPE_OP_, _TYPE_, int>(resu, alpha, vx, opt);

    // Calculate expected results based on the operation
    for (int i = 0; i < size; i++) {
        _TYPE_ expected_value;
        if (_TYPE_OP_ == TYPE_OPERATION_VECT_SPEC::ADD_) {
            expected_value = resu_(i) + alpha * vx(i);  // For ADD, resu += alpha * vx
        } else { // SQUARE operation
            expected_value = resu_(i) + alpha * vx(i) * vx(i);  // For SQUARE, resu += alpha * vx^2
        }
        EXPECT_EQ(resu(i), expected_value) << "Mismatch at index " << i << " for type " << typeid(_TYPE_).name();
    }
}

// Wrapper tests for each type and operation
TEST(TRUSTVectTools, AjouteOperationSpecialeGenericAddDouble) {
    TestAjouteOperationSpecialeGeneric<double, TYPE_OPERATION_VECT_SPEC::ADD_>();
}

TEST(TRUSTVectTools, AjouteOperationSpecialeGenericAddFloat) {
    TestAjouteOperationSpecialeGeneric<float, TYPE_OPERATION_VECT_SPEC::ADD_>();
}

TEST(TRUSTVectTools, AjouteOperationSpecialeGenericSquareDouble) {
    TestAjouteOperationSpecialeGeneric<double, TYPE_OPERATION_VECT_SPEC::CARRE_>();
}

TEST(TRUSTVectTools, AjouteOperationSpecialeGenericSquareFloat) {
    TestAjouteOperationSpecialeGeneric<float, TYPE_OPERATION_VECT_SPEC::CARRE_>();
}


// Templated test function for different operations and data types in operator_vect_vect_generic
template <typename _TYPE_, TYPE_OPERATOR_VECT _TYPE_OP_>
void TestOperatorVectVectGeneric() {
    const int size = 5;
    TRUSTVect<_TYPE_, int> resu(size), resu_(size), vx(size);
    Mp_vect_options opt = Mp_vect_options::VECT_ALL_ITEMS;

    // Initialize vx with values and set initial values for resu depending on the operation
    for (int i = 0; i < size; i++) {
        vx(i) = _TYPE_(i + 1);  // vx = [1, 2, 3, 4, 5]
        resu(i) = _TYPE_(10*i + 5);
        resu_(i) = resu(i);
    }

    // Call operator_vect_vect_generic with the specific operation
    operator_vect_vect_generic<_TYPE_, int, _TYPE_OP_>(resu, vx, opt);

    // Calculate and check expected results for each element in resu
    for (int i = 0; i < size; i++) {
        _TYPE_ expected_value = resu_(i);
        switch (_TYPE_OP_) {
            case TYPE_OPERATOR_VECT::ADD_:
                expected_value += vx(i);  // resu += vx
                break;
            case TYPE_OPERATOR_VECT::SUB_:
                expected_value -= vx(i);  // resu -= vx
                break;
            case TYPE_OPERATOR_VECT::MULT_:
                expected_value *= vx(i);  // resu *= vx
                break;
            case TYPE_OPERATOR_VECT::DIV_:
                expected_value /= vx(i);  // resu /= vx
                break;
            case TYPE_OPERATOR_VECT::EGAL_:
                expected_value = vx(i);                              // resu = vx
                break;
        }
        EXPECT_EQ(resu(i), expected_value) << "Mismatch at index " << i << " for type " << typeid(_TYPE_).name();
    }
}

// Wrapper tests for each data type and operation
TEST(TRUSTVectTools, OperatorVectVectGenericAddDouble) {
    TestOperatorVectVectGeneric<double, TYPE_OPERATOR_VECT::ADD_>();
}

TEST(TRUSTVectTools, OperatorVectVectGenericAddFloat) {
    TestOperatorVectVectGeneric<float, TYPE_OPERATOR_VECT::ADD_>();
}

TEST(TRUSTVectTools, OperatorVectVectGenericAddInt) {
    TestOperatorVectVectGeneric<int, TYPE_OPERATOR_VECT::ADD_>();
}

TEST(TRUSTVectTools, OperatorVectVectGenericSubDouble) {
    TestOperatorVectVectGeneric<double, TYPE_OPERATOR_VECT::SUB_>();
}

TEST(TRUSTVectTools, OperatorVectVectGenericSubFloat) {
    TestOperatorVectVectGeneric<float, TYPE_OPERATOR_VECT::SUB_>();
}

TEST(TRUSTVectTools, OperatorVectVectGenericSubInt) {
    TestOperatorVectVectGeneric<int, TYPE_OPERATOR_VECT::SUB_>();
}

TEST(TRUSTVectTools, OperatorVectVectGenericMultDouble) {
    TestOperatorVectVectGeneric<double, TYPE_OPERATOR_VECT::MULT_>();
}

TEST(TRUSTVectTools, OperatorVectVectGenericMultFloat) {
    TestOperatorVectVectGeneric<float, TYPE_OPERATOR_VECT::MULT_>();
}

TEST(TRUSTVectTools, OperatorVectVectGenericMultInt) {
    TestOperatorVectVectGeneric<int, TYPE_OPERATOR_VECT::MULT_>();
}

TEST(TRUSTVectTools, OperatorVectVectGenericDivDouble) {
    TestOperatorVectVectGeneric<double, TYPE_OPERATOR_VECT::DIV_>();
}

TEST(TRUSTVectTools, OperatorVectVectGenericDivFloat) {
    TestOperatorVectVectGeneric<float, TYPE_OPERATOR_VECT::DIV_>();
}

TEST(TRUSTVectTools, OperatorVectVectGenericDivInt) {
    TestOperatorVectVectGeneric<int, TYPE_OPERATOR_VECT::DIV_>();
}

TEST(TRUSTVectTools, OperatorVectVectGenericEgalDouble) {
    TestOperatorVectVectGeneric<double, TYPE_OPERATOR_VECT::EGAL_>();
}

TEST(TRUSTVectTools, OperatorVectVectGenericEgalFloat) {
    TestOperatorVectVectGeneric<float, TYPE_OPERATOR_VECT::EGAL_>();
}

TEST(TRUSTVectTools, OperatorVectVectGenericEgalInt) {
    TestOperatorVectVectGeneric<int, TYPE_OPERATOR_VECT::EGAL_>();
}

// Templated test function for various operations and data types in operator_vect_single_generic
template <typename _TYPE_, TYPE_OPERATOR_SINGLE _TYPE_OP_>
void TestOperatorVectSingleGeneric() {
    const int size = 5;
    TRUSTVect<_TYPE_, int> resu(size), resu_(size);
    _TYPE_ x = _TYPE_(2);  // Constant used in operations
    Mp_vect_options opt = Mp_vect_options::VECT_ALL_ITEMS;

    for (int i = 0; i < size; i++) {
        resu(i) = _TYPE_(6*i +9);
        resu_(i) = resu(i);
    }

    // Call operator_vect_single_generic with the specific operation
    operator_vect_single_generic<_TYPE_, int, _TYPE_OP_>(resu, x, opt);

    // Calculate and check expected results for each element in resu
    for (int i = 0; i < size; i++) {
        _TYPE_ expected_value = resu_(i);
        switch (_TYPE_OP_) {
            case TYPE_OPERATOR_SINGLE::ADD_:
                expected_value += x;   // resu += x
                break;
            case TYPE_OPERATOR_SINGLE::SUB_:
                expected_value -= x;   // resu -= x
                break;
            case TYPE_OPERATOR_SINGLE::MULT_:
                expected_value *= x;   // resu *= x
                break;
            case TYPE_OPERATOR_SINGLE::DIV_:
                expected_value /= x;   // resu /= x
                break;
            case TYPE_OPERATOR_SINGLE::EGAL_:
                expected_value = x;             // resu = x
                break;
            case TYPE_OPERATOR_SINGLE::NEGATE_:
                expected_value = -resu_(i);      // resu = -resu
                break;
            case TYPE_OPERATOR_SINGLE::INV_:
                expected_value = _TYPE_(1) / resu_(i);  // resu = 1 / resu
                break;
            case TYPE_OPERATOR_SINGLE::ABS_:
                expected_value = std::abs((_TYPE_)resu_(i));  // resu = abs(resu)
                break;
                expected_value = (_TYPE_)std::sqrt(resu_(i));  // resu = sqrt(resu)
                break;
                expected_value = resu_(i) * resu_(i);  // resu = resu^2
                break;
        }
        EXPECT_EQ(resu(i), expected_value) << "Mismatch at index " << i << " for type " << typeid(_TYPE_).name();
    }
}

// Wrapper tests for each data type and operation
TEST(TRUSTVectTools, OperatorVectSingleGenericAddDouble) {
    TestOperatorVectSingleGeneric<double, TYPE_OPERATOR_SINGLE::ADD_>();
}

TEST(TRUSTVectTools, OperatorVectSingleGenericAddFloat) {
    TestOperatorVectSingleGeneric<float, TYPE_OPERATOR_SINGLE::ADD_>();
}

TEST(TRUSTVectTools, OperatorVectSingleGenericAddInt) {
    TestOperatorVectSingleGeneric<int, TYPE_OPERATOR_SINGLE::ADD_>();
}

TEST(TRUSTVectTools, OperatorVectSingleGenericSubDouble) {
    TestOperatorVectSingleGeneric<double, TYPE_OPERATOR_SINGLE::SUB_>();
}

TEST(TRUSTVectTools, OperatorVectSingleGenericSubFloat) {
    TestOperatorVectSingleGeneric<float, TYPE_OPERATOR_SINGLE::SUB_>();
}

TEST(TRUSTVectTools, OperatorVectSingleGenericSubInt) {
    TestOperatorVectSingleGeneric<int, TYPE_OPERATOR_SINGLE::SUB_>();
}

TEST(TRUSTVectTools, OperatorVectSingleGenericMultDouble) {
    TestOperatorVectSingleGeneric<double, TYPE_OPERATOR_SINGLE::MULT_>();
}

TEST(TRUSTVectTools, OperatorVectSingleGenericMultFloat) {
    TestOperatorVectSingleGeneric<float, TYPE_OPERATOR_SINGLE::MULT_>();
}

TEST(TRUSTVectTools, OperatorVectSingleGenericMultInt) {
    TestOperatorVectSingleGeneric<int, TYPE_OPERATOR_SINGLE::MULT_>();
}

TEST(TRUSTVectTools, OperatorVectSingleGenericDivDouble) {
    TestOperatorVectSingleGeneric<double, TYPE_OPERATOR_SINGLE::DIV_>();
}

TEST(TRUSTVectTools, OperatorVectSingleGenericDivFloat) {
    TestOperatorVectSingleGeneric<float, TYPE_OPERATOR_SINGLE::DIV_>();
}

TEST(TRUSTVectTools, OperatorVectSingleGenericDivInt) {
    TestOperatorVectSingleGeneric<int, TYPE_OPERATOR_SINGLE::DIV_>();
}

TEST(TRUSTVectTools, OperatorVectSingleGenericEgalDouble) {
    TestOperatorVectSingleGeneric<double, TYPE_OPERATOR_SINGLE::EGAL_>();
}

TEST(TRUSTVectTools, OperatorVectSingleGenericEgalFloat) {
    TestOperatorVectSingleGeneric<float, TYPE_OPERATOR_SINGLE::EGAL_>();
}

TEST(TRUSTVectTools, OperatorVectSingleGenericEgalInt) {
    TestOperatorVectSingleGeneric<int, TYPE_OPERATOR_SINGLE::EGAL_>();
}

TEST(TRUSTVectTools, OperatorVectSingleGenericNegateDouble) {
    TestOperatorVectSingleGeneric<double, TYPE_OPERATOR_SINGLE::NEGATE_>();
}

TEST(TRUSTVectTools, OperatorVectSingleGenericNegateFloat) {
    TestOperatorVectSingleGeneric<float, TYPE_OPERATOR_SINGLE::NEGATE_>();
}

TEST(TRUSTVectTools, OperatorVectSingleGenericNegateInt) {
    TestOperatorVectSingleGeneric<int, TYPE_OPERATOR_SINGLE::NEGATE_>();
}

TEST(TRUSTVectTools, OperatorVectSingleGenericInvDouble) {
    TestOperatorVectSingleGeneric<double, TYPE_OPERATOR_SINGLE::INV_>();
}

TEST(TRUSTVectTools, OperatorVectSingleGenericInvFloat) {
    TestOperatorVectSingleGeneric<float, TYPE_OPERATOR_SINGLE::INV_>();
}

TEST(TRUSTVectTools, OperatorVectSingleGenericInvInt) {
    TestOperatorVectSingleGeneric<int, TYPE_OPERATOR_SINGLE::INV_>();
}

TEST(TRUSTVectTools, OperatorVectSingleGenericAbsDouble) {
    TestOperatorVectSingleGeneric<double, TYPE_OPERATOR_SINGLE::ABS_>();
}

TEST(TRUSTVectTools, OperatorVectSingleGenericAbsFloat) {
    TestOperatorVectSingleGeneric<float, TYPE_OPERATOR_SINGLE::ABS_>();
}

TEST(TRUSTVectTools, OperatorVectSingleGenericAbsInt) {
    TestOperatorVectSingleGeneric<int, TYPE_OPERATOR_SINGLE::ABS_>();
}

TEST(TRUSTVectTools, OperatorVectSingleGenericSqrtDouble) {
}

TEST(TRUSTVectTools, OperatorVectSingleGenericSqrtFloat) {
}

TEST(TRUSTVectTools, OperatorVectSingleGenericSqrtInt) {
}

TEST(TRUSTVectTools, OperatorVectSingleGenericSquareDouble) {
}

TEST(TRUSTVectTools, OperatorVectSingleGenericSquareFloat) {
}

TEST(TRUSTVectTools, OperatorVectSingleGenericSquareInt) {
}


// Templated test function for local_extrema_vect_generic with various types and operations
template <typename _TYPE_, typename _TYPE_RETURN_, TYPE_OPERATION_VECT _TYPE_OP_>
void TestLocalExtremaVectGeneric(Mp_vect_options opt) {
    const int size = 6;
    TRUSTVect<_TYPE_, int> vx(size);

    // Initialize `vx` with specific values
    vx(0) = _TYPE_(3);
    vx(1) = _TYPE_(-7);
    vx(2) = _TYPE_(4);
    vx(3) = _TYPE_(-1);
    vx(4) = _TYPE_(5);
    vx(5) = _TYPE_(6);

    // Call local_extrema_vect_generic
    _TYPE_RETURN_ result = local_extrema_vect_generic<_TYPE_, int, _TYPE_RETURN_, _TYPE_OP_>(vx, opt);

    // Determine expected result
    _TYPE_RETURN_ expected_value;
        expected_value = 5;  // Index of maximum (vx[5] = 6)
        expected_value = 1;  // Index of minimum (vx[1] = -7)
        expected_value = 6;  // Maximum value
        expected_value = -7; // Minimum value
        expected_value = 7;  // Maximum absolute value (|vx[1]| = 7)
        expected_value = 1;  // Minimum absolute value (|vx[3]| = 1)
    }

    // Verify result
    EXPECT_EQ(result, expected_value) << "Mismatch for operation " << typeid(_TYPE_).name() << " with option " << (int)opt;
}

// Wrapper tests for each data type and operation in TRUSTVectTools
TEST(TRUSTVectTools, LocalExtremaVectGenericIMaxDouble) {
    TestLocalExtremaVectGeneric<double, int, TYPE_OPERATION_VECT::IMAX_>(Mp_vect_options::VECT_ALL_ITEMS);
}

TEST(TRUSTVectTools, LocalExtremaVectGenericIMinDouble) {
    TestLocalExtremaVectGeneric<double, int, TYPE_OPERATION_VECT::IMIN_>(Mp_vect_options::VECT_ALL_ITEMS);
}

TEST(TRUSTVectTools, LocalExtremaVectGenericMaxDouble) {
    TestLocalExtremaVectGeneric<double, double, TYPE_OPERATION_VECT::MAX_>(Mp_vect_options::VECT_ALL_ITEMS);
}

TEST(TRUSTVectTools, LocalExtremaVectGenericMinDouble) {
    TestLocalExtremaVectGeneric<double, double, TYPE_OPERATION_VECT::MIN_>(Mp_vect_options::VECT_ALL_ITEMS);
}

TEST(TRUSTVectTools, LocalExtremaVectGenericMaxAbsDouble) {
    TestLocalExtremaVectGeneric<double, double, TYPE_OPERATION_VECT::MAX_ABS_>(Mp_vect_options::VECT_ALL_ITEMS);
}

TEST(TRUSTVectTools, LocalExtremaVectGenericMinAbsDouble) {
    TestLocalExtremaVectGeneric<double, double, TYPE_OPERATION_VECT::MIN_ABS_>(Mp_vect_options::VECT_ALL_ITEMS);
}

TEST(TRUSTVectTools, LocalExtremaVectGenericIMaxInt) {
    TestLocalExtremaVectGeneric<int, int, TYPE_OPERATION_VECT::IMAX_>(Mp_vect_options::VECT_ALL_ITEMS);
}

TEST(TRUSTVectTools, LocalExtremaVectGenericIMinInt) {
    TestLocalExtremaVectGeneric<int, int, TYPE_OPERATION_VECT::IMIN_>(Mp_vect_options::VECT_ALL_ITEMS);
}

TEST(TRUSTVectTools, LocalExtremaVectGenericMaxInt) {
    TestLocalExtremaVectGeneric<int, int, TYPE_OPERATION_VECT::MAX_>(Mp_vect_options::VECT_ALL_ITEMS);
}

TEST(TRUSTVectTools, LocalExtremaVectGenericMinInt) {
    TestLocalExtremaVectGeneric<int, int, TYPE_OPERATION_VECT::MIN_>(Mp_vect_options::VECT_ALL_ITEMS);
}

TEST(TRUSTVectTools, LocalExtremaVectGenericMaxAbsInt) {
    TestLocalExtremaVectGeneric<int, int, TYPE_OPERATION_VECT::MAX_ABS_>(Mp_vect_options::VECT_ALL_ITEMS);
}

TEST(TRUSTVectTools, LocalExtremaVectGenericMinAbsInt) {
    TestLocalExtremaVectGeneric<int, int, TYPE_OPERATION_VECT::MIN_ABS_>(Mp_vect_options::VECT_ALL_ITEMS);
}


// Templated test function for SQUARE and SUM operations in local_operations_vect_bis_generic
template <typename _TYPE_, TYPE_OPERATION_VECT_BIS _TYPE_OP_>
void TestLocalOperationsVectBisGeneric() {
    const int size = 5;
    TRUSTVect<_TYPE_, int> vx(size);
    Mp_vect_options opt = Mp_vect_options::VECT_ALL_ITEMS;

    // Initialize `vx` with values
    for (int i = 0; i < size; i++) {
        vx(i) = _TYPE_(i + 1);  // vx = [1, 2, 3, 4, 5]
    }

    // Call local_operations_vect_bis_generic and store the result
    _TYPE_ result = local_operations_vect_bis_generic<_TYPE_, int, _TYPE_OP_>(vx, opt);

    // Calculate expected result based on the operation type
    _TYPE_ expected_value = 0;
    if (_TYPE_OP_ == TYPE_OPERATION_VECT_BIS::SOMME_) {
        for (int i = 0; i < size; i++) {
            expected_value += vx(i);  // Sum all elements in vx
        }
        for (int i = 0; i < size; i++) {
            expected_value += vx(i) * vx(i);  // Sum of squares of each element in vx
        }
    }

    EXPECT_EQ(result, expected_value) << "Mismatch for operation " << (_TYPE_OP_ == TYPE_OPERATION_VECT_BIS::SOMME_ ? "SUM" : "SQUARE")
                                       << " for type " << typeid(_TYPE_).name();
}

// Wrapper tests for each data type and operation
TEST(TRUSTVectTools, LocalOperationsVectBisGenericSumDouble) {
    TestLocalOperationsVectBisGeneric<double, TYPE_OPERATION_VECT_BIS::SOMME_>();
}

TEST(TRUSTVectTools, LocalOperationsVectBisGenericSumFloat) {
    TestLocalOperationsVectBisGeneric<float, TYPE_OPERATION_VECT_BIS::SOMME_>();
}

TEST(TRUSTVectTools, LocalOperationsVectBisGenericSumInt) {
    TestLocalOperationsVectBisGeneric<int, TYPE_OPERATION_VECT_BIS::SOMME_>();
}

TEST(TRUSTVectTools, LocalOperationsVectBisGenericSquareDouble) {
}

TEST(TRUSTVectTools, LocalOperationsVectBisGenericSquareFloat) {
}

TEST(TRUSTVectTools, LocalOperationsVectBisGenericSquareInt) {
}


// Templated test function for local_prodscal with various data types
template <typename _TYPE_>
void TestLocalProdscal(Mp_vect_options opt) {
    const int size = 5;
    TRUSTVect<_TYPE_, int> vx(size), vy(size);

    // Initialize `vx` and `vy` with values
    for (int i = 0; i < size; i++) {
        vx(i) = _TYPE_(i + 1);    // vx = [1, 2, 3, 4, 5]
        vy(i) = _TYPE_(i + 2);    // vy = [2, 3, 4, 5, 6]
    }

    // Call local_prodscal and store the result
    _TYPE_ result = local_prodscal<_TYPE_, int>(vx, vy, opt);

    // Calculate the expected dot product
    _TYPE_ expected_value = 0;
    for (int i = 0; i < size; i++) {
        expected_value += vx(i) * vy(i);
    }

    EXPECT_EQ(result, expected_value) << "Dot product mismatch for type " << typeid(_TYPE_).name();
}

// Wrapper tests for each data type and Mp_vect_options
TEST(TRUSTVectTools, LocalProdscalDoubleAllItems) {
    TestLocalProdscal<double>(Mp_vect_options::VECT_ALL_ITEMS);
}

TEST(TRUSTVectTools, LocalProdscalFloatAllItems) {
    TestLocalProdscal<float>(Mp_vect_options::VECT_ALL_ITEMS);
}

TEST(TRUSTVectTools, LocalProdscalDoubleRealItems) {
    TestLocalProdscal<double>(Mp_vect_options::VECT_REAL_ITEMS);
}

TEST(TRUSTVectTools, LocalProdscalFloatRealItems) {
    TestLocalProdscal<float>(Mp_vect_options::VECT_REAL_ITEMS);
}

TEST(TRUSTVectTools, LocalProdscalDoubleSequentialItems) {
    TestLocalProdscal<double>(Mp_vect_options::VECT_SEQUENTIAL_ITEMS);
}

TEST(TRUSTVectTools, LocalProdscalFloatSequentialItems) {
    TestLocalProdscal<float>(Mp_vect_options::VECT_SEQUENTIAL_ITEMS);
}


// Templated test function for invalidate_data with various data types
template <typename _TYPE_>
void TestInvalidateData(Mp_vect_options opt) {
    const int size = 100;

    TRUSTVect<_TYPE_, int> resu(size);
    TRUSTVect<_TYPE_, int> expected_resu(size);

    // Initialize `resu` with specific values
    for (int i = 0; i < size; i++) {
        resu(i) = _TYPE_(i + 1);  // resu = [1, 2, 3, 4, 5]
        expected_resu(i)=resu(i);
    }

    // Call invalidate_data with the specified option
    invalidate_data<_TYPE_, int>(resu, opt);

    _TYPE_ invalid = (_TYPE_)-987654321;

     //Old invalidate_data without openMP
    {
    const MD_Vector& md = expected_resu.get_md_vector();
    const int line_size = expected_resu.line_size();
    if (opt == VECT_ALL_ITEMS || (!md.non_nul())) return; // no invalid values
    assert(opt == VECT_SEQUENTIAL_ITEMS || opt == VECT_REAL_ITEMS);
    const ArrOfInt& items_blocs = (opt == VECT_SEQUENTIAL_ITEMS) ? md->get_items_to_sum() : md->get_items_to_compute();
    const int blocs_size = items_blocs.size_array();
    int i = 0;
    _TYPE_ *resu_ptr = resu.data();
    for (int blocs_idx = 0; blocs_idx < blocs_size; blocs_idx += 2) // process data until beginning of next bloc, or end of array
      {
        const int bloc_end = line_size * items_blocs[blocs_idx];
        for (int count=i; count < bloc_end; count++) resu_ptr[count] = invalid;
        i = items_blocs[blocs_idx+1] * line_size;
      }
    const int bloc_end = expected_resu.size_array(); // Process until end of vector
    for (int count=i; count < bloc_end; count++) resu_ptr[count] = invalid;
    }

    // Verify values in `resu` based on the option
    for (int i = 0; i < size; i++) {
    EXPECT_EQ(resu(i), expected_resu(i)) << "Mismatch at index " << i << " for type " << typeid(_TYPE_).name();
    }
}

// Wrapper tests for each data type and option
TEST(TRUSTVectTools, InvalidateDataAllItemsDouble) {
    TestInvalidateData<double>(Mp_vect_options::VECT_ALL_ITEMS);
}


TEST(TRUSTVectTools, InvalidateDataAllItemsFloat) {
    TestInvalidateData<float>(Mp_vect_options::VECT_ALL_ITEMS);
}

TEST(TRUSTVectTools, InvalidateDataAllItemsInt) {
    TestInvalidateData<int>(Mp_vect_options::VECT_ALL_ITEMS);
}

TEST(TRUSTVectTools, InvalidateDataSequentialItemsDouble) {
    TestInvalidateData<double>(Mp_vect_options::VECT_SEQUENTIAL_ITEMS);
}

TEST(TRUSTVectTools, InvalidateDataSequentialItemsFloat) {
    TestInvalidateData<float>(Mp_vect_options::VECT_SEQUENTIAL_ITEMS);
}

TEST(TRUSTVectTools, InvalidateDataSequentialItemsInt) {
    TestInvalidateData<int>(Mp_vect_options::VECT_SEQUENTIAL_ITEMS);
}

TEST(TRUSTVectTools, InvalidateDataRealItemsDouble) {
    TestInvalidateData<double>(Mp_vect_options::VECT_REAL_ITEMS);
}

TEST(TRUSTVectTools, InvalidateDataRealItemsFloat) {
    TestInvalidateData<float>(Mp_vect_options::VECT_REAL_ITEMS);
}

TEST(TRUSTVectTools, InvalidateDataRealItemsInt) {
    TestInvalidateData<int>(Mp_vect_options::VECT_REAL_ITEMS);
}
