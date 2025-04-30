function [M_B_O, M_B_O_dphi, M_B_O_dtheta, M_B_O_dpsi] = get_M_B_O(phi, theta, psi)
% Outputs linearization of the M_B_O rotation

% Rotation matrices
[Mx, My, Mz] = get_rot_matrix(phi, theta, psi);

% Define M_B_O
M_B_O = Mx*My*Mz;

% Derivative of rotation matrices
Mx_dphi = [0, 0, 0; 0, -sin(phi), cos(phi); 0, -cos(phi), -sin(phi)];
My_dtheta = [-sin(theta), 0, -cos(theta); 0, 0, 0; cos(theta), 0, -sin(theta)];
Mz_dpsi = [-sin(psi), cos(psi), 0; -cos(psi), -sin(psi), 0; 0, 0, 0];

% Derivative of transformation
M_B_O_dphi      = Mx_dphi * My        * Mz;
M_B_O_dtheta    = Mx      * My_dtheta * Mz;
M_B_O_dpsi      = Mx      * My        * Mz_dpsi;

end