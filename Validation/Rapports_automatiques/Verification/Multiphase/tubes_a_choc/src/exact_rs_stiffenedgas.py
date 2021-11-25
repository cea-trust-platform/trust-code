# -*-coding:utf-8 -*

######################################################################################################################
# 	This file contains a class to solve for the exact solution of the Riemann Problem for the one dimensional Euler
# 	equations with stiffened gas equation of state
#
# 	Author: Michael Ndjinga
# 	Date:	18/02/2021
#   Description : Translated from C++ package developped by Murray Cutforth
#######################################################################################################################

from math import pow, fabs, sqrt


class exact_rs_stiffenedgas:
    def __init__(self, gamma_L, gamma_R, pinf_L, pinf_R, tol=1.0e-6, max_iter=100):
        self.TOL = tol
        self.MAX_NB_ITER = max_iter

        self.gamma_L = gamma_L
        self.gamma_R = gamma_R
        self.pinf_L = pinf_L
        self.pinf_R = pinf_R

        self.S_STAR = 0.0
        self.P_STAR = 0.0
        self.rho_star_L = 0.0
        self.rho_star_R = 0.0

        self.S_L = 0.0
        self.S_R = 0.0
        self.S_HL = 0.0
        self.S_TL = 0.0
        self.S_HR = 0.0
        self.S_TR = 0.0

    # Functions used to generate exact solutions to Riemann problems

    def solve_RP(self, W_L, W_R):
        assert len(W_L) == 3, "Left state should have three components (rho, u p)"
        assert len(W_R) == 3, "Right state should have three components (rho, u p)"
        assert W_L[0] >= 0.0, "Left density should be positive"
        assert W_R[0] >= 0.0, "Right density should be positive"
        # assert W_L[2] >= 0.0 # Since stiffened gases will often exhibit p<0..
        # assert W_R[2] >= 0.0 #

        print("")
        print("Solving Riemann problem for left state W_L=", W_L, ", and right state W_R=", W_R)

        # Calculate p_star

        self.P_STAR = self.find_p_star_newtonraphson(W_L[0], W_L[1], W_L[2], W_R[0], W_R[1], W_R[2])

        # Calculate u_star

        self.S_STAR = 0.5 * (W_L[1] + W_R[1]) + 0.5 * (
            self.f(self.P_STAR, W_R[0], W_R[2], self.gamma_R, self.pinf_R)
            - self.f(self.P_STAR, W_L[0], W_L[2], self.gamma_L, self.pinf_L)
        )

        # Solution now depends on character of 1st and 3rd waves

        if self.P_STAR > W_L[2]:
            # Left shock

            self.rho_star_L = W_L[0] * (
                (2.0 * self.gamma_L * self.pinf_L + (self.gamma_L + 1.0) * self.P_STAR + (self.gamma_L - 1.0) * W_L[2])
                / (
                    2.0 * (W_L[2] + self.gamma_L * self.pinf_L)
                    + (self.gamma_L - 1.0) * self.P_STAR
                    + (self.gamma_L - 1.0) * W_L[2]
                )
            )
            self.S_L = W_L[1] - (self.Q_K(self.P_STAR, W_L[0], W_L[2], self.gamma_L, self.pinf_L) / W_L[0])
        else:
            # Left rarefaction

            self.rho_star_L = W_L[0] * pow((self.P_STAR + self.pinf_L) / (W_L[2] + self.pinf_L), 1.0 / self.gamma_L)

            a_L = self.a(W_L[0], W_L[2], self.gamma_L, self.pinf_L)
            a_star_L = a_L * pow(
                (self.P_STAR + self.pinf_L) / (W_L[2] + self.pinf_L), (self.gamma_L - 1.0) / (2.0 * self.gamma_L)
            )

            self.S_HL = W_L[1] - a_L
            self.S_TL = self.S_STAR - a_star_L

        if self.P_STAR > W_R[2]:
            # Right shock

            self.rho_star_R = W_R[0] * (
                (2.0 * self.gamma_R * self.pinf_R + (self.gamma_R + 1.0) * self.P_STAR + (self.gamma_R - 1.0) * W_R[2])
                / (
                    2.0 * (W_R[2] + self.gamma_R * self.pinf_R)
                    + (self.gamma_R - 1.0) * self.P_STAR
                    + (self.gamma_R - 1.0) * W_R[2]
                )
            )

            self.S_R = W_R[1] + (self.Q_K(self.P_STAR, W_R[0], W_R[2], self.gamma_R, self.pinf_R) / W_R[0])
        else:
            # Right rarefaction

            self.rho_star_R = W_R[0] * pow((self.P_STAR + self.pinf_R) / (W_R[2] + self.pinf_R), 1.0 / self.gamma_R)

            a_R = self.a(W_R[0], W_R[2], self.gamma_R, self.pinf_R)
            a_star_R = a_R * pow(
                (self.P_STAR + self.pinf_R) / (W_R[2] + self.pinf_R), (self.gamma_R - 1.0) / (2.0 * self.gamma_R)
            )

            self.S_HR = W_R[1] + a_R
            self.S_TR = self.S_STAR + a_star_R

    def sample_solution(self, W_L, W_R, S):
        W = [0.0] * 3

        # Find appropriate part of solution and return primitives

        if S < self.S_STAR:
            # To the left of the contact

            if self.P_STAR > W_L[2]:
                # Left shock

                if S < self.S_L:
                    W = W_L
                else:
                    W[0] = self.rho_star_L
                    W[1] = self.S_STAR
                    W[2] = self.P_STAR
            else:
                # Left rarefaction

                if S < self.S_HL:
                    W = W_L
                else:
                    if S > self.S_TL:
                        W[0] = self.rho_star_L
                        W[1] = self.S_STAR
                        W[2] = self.P_STAR
                    else:
                        self.set_left_rarefaction_fan_state(W_L, S, W)
        else:
            # To the right of the contact

            if self.P_STAR > W_R[2]:
                # Right shock

                if S > self.S_R:
                    W = W_R
                else:
                    W[0] = self.rho_star_R
                    W[1] = self.S_STAR
                    W[2] = self.P_STAR
            else:
                # Right rarefaction

                if S > self.S_HR:
                    W = W_R
                else:
                    if S < self.S_TR:
                        W[0] = self.rho_star_R
                        W[1] = self.S_STAR
                        W[2] = self.P_STAR
                    else:
                        self.set_right_rarefaction_fan_state(W_R, S, W)

        return W

    # Functions used to solve for p_star iteratively

    def find_p_star_newtonraphson(self, rho_L, u_L, p_L, rho_R, u_R, p_R):

        # First we set the initial guess for p_star using a simple mean-value approximation

        p_star_next = 0.5 * (p_L + p_R)
        n = 0

        # Now use the Newton-Raphson algorithm

        while True:  # conversion of do ... while by while True... if (...) break
            p_star = p_star_next

            p_star_next = p_star - self.total_pressure_function(
                p_star, rho_L, u_L, p_L, rho_R, u_R, p_R
            ) / self.total_pressure_function_deriv(p_star, rho_L, p_L, rho_R, p_R)

            p_star_next = max(p_star_next, self.TOL)

            n += 1

            if not ((fabs(p_star_next - p_star) / (0.5 * (p_star + p_star_next)) > self.TOL) and n < self.MAX_NB_ITER):
                break

        if n == self.MAX_NB_ITER:
            raise ValueError(
                "!!!!!!!!!!Newton algorithm did not converge. Increase tolerance or maximum number of time steps. Current values : tol="
                + str(self.TOL)
                + ", max_iter="
                + str(self.MAX_NB_ITER)
            )
            # p_star_next = 0.5*(p_L+p_R)

        return p_star_next

    def total_pressure_function(self, p_star, rho_L, u_L, p_L, rho_R, u_R, p_R):

        return (
            self.f(p_star, rho_L, p_L, self.gamma_L, self.pinf_L)
            + self.f(p_star, rho_R, p_R, self.gamma_R, self.pinf_R)
            + u_R
            - u_L
        )

    def total_pressure_function_deriv(self, p_star, rho_L, p_L, rho_R, p_R):

        return self.f_deriv(p_star, rho_L, p_L, self.gamma_L, self.pinf_L) + self.f_deriv(
            p_star, rho_R, p_R, self.gamma_R, self.pinf_R
        )

    def f(self, p_star, rho, p, gamma, pinf):
        if p_star > p:

            return (p_star - p) / self.Q_K(p_star, rho, p, gamma, pinf)

        else:

            return (2.0 * self.a(rho, p, gamma, pinf) / (gamma - 1.0)) * (
                pow((p_star + pinf) / (p + pinf), (gamma - 1.0) / (2.0 * gamma)) - 1.0
            )

    def f_deriv(self, p_star, rho, p, gamma, pinf):
        A = 2.0 / ((gamma + 1.0) * rho)
        B = (p + pinf) * (gamma - 1.0) / (gamma + 1.0)

        if p_star > p:

            return sqrt(A / (B + p_star + pinf)) * (1.0 - ((p_star - p) / (2.0 * (B + p_star + pinf))))

        else:

            return (1.0 / (rho * self.a(rho, p, gamma, pinf))) * pow(
                (p_star + pinf) / (p + pinf), -(gamma + 1.0) / (2.0 * gamma)
            )

    # Functions to find the state inside a rarefaction fan

    def set_left_rarefaction_fan_state(self, W_L, S, W):
        a_L = self.a(W_L[0], W_L[2], self.gamma_L, self.pinf_L)
        W[0] = W_L[0] * pow(
            (2.0 / (self.gamma_L + 1.0)) + ((self.gamma_L - 1.0) / (a_L * (self.gamma_L + 1.0))) * (W_L[1] - S),
            2.0 / (self.gamma_L - 1.0),
        )
        W[1] = (2.0 / (self.gamma_L + 1.0)) * (a_L + S + ((self.gamma_L - 1.0) / 2.0) * W_L[1])
        W[2] = (W_L[2] + self.pinf_L) * pow(
            (2.0 / (self.gamma_L + 1.0)) + ((self.gamma_L - 1.0) / (a_L * (self.gamma_L + 1.0))) * (W_L[1] - S),
            (2.0 * self.gamma_L) / (self.gamma_L - 1.0),
        ) - self.pinf_L

    def set_right_rarefaction_fan_state(self, W_R, S, W):
        a_R = self.a(W_R[0], W_R[2], self.gamma_R, self.pinf_R)
        W[0] = W_R[0] * pow(
            (2.0 / (self.gamma_R + 1.0)) - ((self.gamma_R - 1.0) / (a_R * (self.gamma_R + 1.0))) * (W_R[1] - S),
            2.0 / (self.gamma_R - 1.0),
        )
        W[1] = (2.0 / (self.gamma_R + 1.0)) * (-a_R + S + ((self.gamma_R - 1.0) / 2.0) * W_R[1])
        W[2] = (W_R[2] + self.pinf_R) * pow(
            (2.0 / (self.gamma_R + 1.0)) - ((self.gamma_R - 1.0) / (a_R * (self.gamma_R + 1.0))) * (W_R[1] - S),
            (2.0 * self.gamma_R) / (self.gamma_R - 1.0),
        ) - self.pinf_R

    # Misc functions

    def Q_K(self, p_star, rho, p, gamma, pinf):
        A = 2.0 / ((gamma + 1.0) * rho)
        B = (p + pinf) * (gamma - 1.0) / (gamma + 1.0)
        return sqrt((p_star + pinf + B) / A)

    # Equation of state functions

    def a(self, rho, p, gamma, pinf):  # sound speed
        return sqrt(gamma * ((p + pinf) / rho))
