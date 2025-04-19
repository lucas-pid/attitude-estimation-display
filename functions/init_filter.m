clc; clear
%% Load sensor data
% temp = load('sens_data.mat');
% sens = temp.sens;

%% Initialize model parameters
sim.sample_time_s = 1/100;

%% Initialize filter parameters

% A matrix
% Assumption: Small angle approximation
% States: Euler, acc, g, rot_bias, acc_bias
A = [zeros(3,3), zeros(3,4), -eye(3,3), zeros(3,3)
    zeros(10,13)];

% B Matrix
B = [eye(3)
    zeros(10,3)];

% Get O to B transformation and derivatives
% Assume small angle
phi     = 0;
theta   = 0;
psi     = 0;

% Derivatives of M_BO transformation
[M_B_O, M_B_O_dphi, M_B_O_dtheta, M_B_O_dpsi] = get_M_B_O(phi, theta, psi);

% Assumed gravity value
g0 = [0;0;-9.81];

% Observation matrix
C = [M_B_O_dphi*g0, M_B_O_dtheta*g0, M_B_O_dpsi*g0, eye(3), g0, zeros(3,3), eye(3)];

% Disturbances B matrix
% Disturbances: gyro noise, acceleration noise, gyro bias noise, acc bias noise
B_w     = [ eye(3),     zeros(3,9)                          % Euler
            zeros(3,3), eye(3),     zeros(3,6)              % Acc
            zeros(1,12)                                     % g
            zeros(3,3), zeros(3,3), eye(3), zeros(3,3)      % Rot bias
            zeros(3,3), zeros(3,3), zeros(3,3), eye(3)];    % Acc bias

% Process covariance
rot_var         = 9.1385e-5 * ones(3,1);
acc_var         = 0.0096236 * ones(3,1);
rot_bias_var    = 3.0462e-9 * ones(3,1);
acc_bias_var    = 3.0462e-9 * ones(3,1);

Q = diag([rot_var; acc_var*1e2; rot_bias_var; acc_bias_var]);

% Measurement covariance
R = diag(acc_var);

% Assemble filter struct with all matrices
filt.sample_time_s = 1/100;
filt.Phi        = expm(A * filt.sample_time_s);
filt.Gamma_u    = B   * filt.sample_time_s;
filt.Gamma_w    = B_w * filt.sample_time_s;
filt.Q          = Q;
filt.R          = diag(R);
bias_gyro       = [0.001371 -0.000722 -0.000064]';
bias_acc        = [-0.000015 0.000080 -0.000334]';
filt.x0         = [deg2rad([9.8; -5.2; 0]); zeros(3,1); [-9.81]; bias_gyro; bias_acc];
filt.C          = C;
filt.I_n        = eye(length(A(:,1)));
filt.x_num      = length(filt.Phi(:,1));
filt.P0         = zeros(filt.x_num,filt.x_num);
filt.Gamma_w_times_Q = diag(filt.Gamma_w * filt.Q * filt.Gamma_w');

% Filter states idx
filt.eul_idx = uint8(1:3);
filt.acc_idx = uint8(4:6);
filt.g_idx = uint8(7);
filt.rot_bias_idx = uint8(8:10);
filt.acc_bias_idx = uint8(11:13);
filt.rot_noise_idx = uint8(1:3);

filt            = double2single(filt);
% filt.sample_time_s = 1/100;

%% Auxiliar
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

function [Mx, My, Mz] = get_rot_matrix(phi, theta, psi)
Mx = [1,0,0; 0, cos(phi), sin(phi); 0, -sin(phi), cos(phi)];
My = [cos(theta), 0, -sin(theta); 0, 1, 0; sin(theta), 0, cos(theta)];
Mz = [cos(psi), sin(psi), 0; -sin(psi), cos(psi), 0; 0, 0, 1];
end

function in_struct = double2single(in_struct)

field_names = fieldnames(in_struct);

for ii = 1:numel(field_names)


    val = in_struct.(field_names{ii});

    if isa(val, 'double')
        in_struct.(field_names{ii}) = single(val);
    end
end

end