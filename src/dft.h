/**************************************************************************
 *   dft.h  --  This file is part of DFTCXX.                              *
 *                                                                        *
 *   Copyright (C) 2016, Ivo Filot                                        *
 *                                                                        *
 *   DFTCXX is free software:                                             *
 *   you can redistribute it and/or modify it under the terms of the      *
 *   GNU General Public License as published by the Free Software         *
 *   Foundation, either version 3 of the License, or (at your option)     *
 *   any later version.                                                   *
 *                                                                        *
 *   DFTCXX is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty          *
 *   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.              *
 *   See the GNU General Public License for more details.                 *
 *                                                                        *
 *   You should have received a copy of the GNU General Public License    *
 *   along with this program.  If not, see http://www.gnu.org/licenses/.  *
 *                                                                        *
 **************************************************************************/

#ifndef _DFT_H
#define _DFT_H

#include <Eigen/Dense>
#include <boost/math/special_functions/factorials.hpp>
#include <boost/format.hpp>

#include "molecule.h"
#include "moleculargrid.h"
#include "integrals.h"
#include "functionals.h"

typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> MatrixXXd;
typedef Eigen::Matrix<double, Eigen::Dynamic, 1> VectorXd;

class DFT {
public:
    /**
     * @fn DFT
     * @brief DFT routine constructor
     *
     * @return DFT instance
     */
    DFT();

    /**
     * @fn add_molecule
     * @brief add molecule to the DFT routine
     *
     * @param _mol  pointer to Molecule class
     *
     * Links molecule to the DFT routine by a pointer. The
     * Contracted Gaussian Functions of the molecule are copied
     * to the DFT routine class and the routine matrices are
     * constructed.
     *
     * @return void
     */
    void add_molecule(const Molecule* _mol);

    /**
     * @fn scf
     * @brief perform the self-consistent field iterations
     *
     * @return void
     */
    void scf();

private:
    static constexpr double pi = 3.14159265358979323846;

    const Molecule* mol;        // pointer to molecule class
    MolecularGrid* molgrid;     // molecular grid
    Integrator* integrator;     // pointer to integrator class
    Functional* functional;     // pointer to functional class

    std::vector<const CGF*> cgfs;   // vector of Contracted Gaussian Functions

    MatrixXXd S;        // overlap matrix
    MatrixXXd T;        // kinetic matrix
    MatrixXXd V;        // nuclear attraction  matrix
    MatrixXXd H;        // single electron matrix
    VectorXd  ints;     // two-electron integrals

    MatrixXXd X;        // transformation matrix
    MatrixXXd Xp;       // transpose of transformation matrix
    MatrixXXd Cc;       // basis set eigenvectors in transformed basis
    MatrixXXd C;        // basis set eigenvectors in original basis
    MatrixXXd P;        // density matrix
    MatrixXXd J;        // two-electron (coulomb) repulsion matrix
    MatrixXXd XC;       // two-electron exchange-correlation matrix

    unsigned int nelec; // number of electrons of the molecule

    double exc;         // exchange correlation energy
    double enuc;        // nuclear repulsion energy
    double et;          // total energy

    bool is_first;      // whether this is the first iteration

    /*
     * construction functions
     */

    /**
     * @fn copy_cgfs_from_molecule
     * @brief copy cgfs from the molecule class to the dft class
     *
     * @return void
     */
    void copy_cgfs_from_molecule();

    /**
     * @fn construct_matrices
     * @brief construct the matrices and two-electron integrals for the SCF procedure
     *
     * @return void
     */
    void construct_matrices();

    /**
     * @fn calculate_nuclear_repulsion
     * @brief calculate the nuclear repulsion energy
     *
     * @return void
     */
    void calculate_nuclear_repulsion();

    /**
     * @fn calculate_two_electron_integrals
     * @brief calculate the two-electron integrals
     *
     * @return void
     */
    void calculate_two_electron_integrals();

    /**
     * @fn calculate_transformation_matrix
     * @brief calculate the transformation matrix X from the overlap matrix S
     *
     * The transformation matrix X is calculated from the overlap matrix S by
     * canonical orthogonalization. This transformation matrix X ensures that
     * the Slater determinant consists of orthogonal spin-orbitals.
     *
     * @return void
     */
    void calculate_transformation_matrix();

    /*
     * functions for the SCF routine
     */

    /**
     * @fn calculate_density_matrix
     * @brief calculate the density matrix from the coefficient matrix
     *
     * Performs eigenvector decomposition on the Fock matrix F. From these
     * the coefficients in the transformed basis are calculated. The basis
     * function coefficients in the original basis can be obtained by
     * using the transformation matrix X. From these coefficients in the
     * original representation, the density matrix P is calculated.
     *
     * @return void
     */
    void calculate_density_matrix();

    /**
     * @fn calculate_electronic_repulsion_matrix
     * @brief calculates electronic repulsion matrix
     *
     * Calculate the electronic repulsion matrix using the
     * density matrix and the two-electron integrals.
     *
     * @return void
     */
    void calculate_electronic_repulsion_matrix();

    /**
     * @fn calculate_exchange_correlation_matrix
     * @brief calculates the exchange-correlation matrix
     *
     * @return void
     */
    void calculate_exchange_correlation_matrix();

    /**
     * @fn calculate_energy
     * @brief calculate the energy of the molecule
     *
     * @return void
     */
    void calculate_energy();
};

#endif //_DFT_H