clc; clear

%% Initialize model parameters
sim.sample_time_s = 1/100;

% Load buses
load_buses;

% Load model configuration
configuration_filter = filter_configuration();

%% Initialize filter parameters

% A matrix
% Assumption: Small angle approximation
% States:   Euler,              g,          rot_bias,       acc_bias,       linear_acceleration
A = [       zeros(3,3),     zeros(3,1),     -eye(3,3),      zeros(3,3),         zeros(3,3)
            zeros(7,13)
            zeros(3,10),                                                        -eye(3)*0.5    ];

% B Matrix
B = [   eye(3)
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
C = [M_B_O_dphi*g0, M_B_O_dtheta*g0, M_B_O_dpsi*g0, g0, zeros(3,3), eye(3), eye(3)];

% Disturbances B matrix
% Disturbances: gyro noise,     Rot bias noise,     acc bias noise,     linear acceleration noise 
B_w     = [     eye(3),         zeros(3,9)                                                      % Euler
                zeros(1,12)                                                                     % g
                zeros(3,3),     eye(3),             zeros(3,6)                                  % Rot bias
                zeros(3,3),     zeros(3,3),         eye(3),             zeros(3,3)              % Acc bias
                zeros(3,9),                                             eye(3)];                % Linear acceleration

% Process covariance
rot_var         = 9e-5 * ones(3,1);
meas_acc_var    = 9e-3 * ones(3,1);
lin_acc_var     = 9e-2 * ones(3,1);
rot_bias_var    = 1e-8 * ones(3,1);
acc_bias_var    = 1e-8 * ones(3,1);

Q = diag([rot_var; rot_bias_var; acc_bias_var; lin_acc_var]);

% Measurement covariance
R = diag(meas_acc_var);

% Assemble filter struct with all matrices
filt.sample_time_s = 1/100;
filt.Phi        = expm(A * filt.sample_time_s);
filt.Gamma_u    = B   * filt.sample_time_s;
filt.Gamma_w    = B_w * filt.sample_time_s;
filt.Q          = Q;
filt.R          = R;
bias_gyro       = [0 0 0]';
bias_acc        = [0 0 0]';
g               = -9.81;
filt.x0         = [deg2rad([0; 0; 0]); g; bias_gyro; bias_acc; zeros(3,1)];
filt.C          = C;
filt.I_n        = eye(length(A(:,1)));
filt.x_num      = length(filt.Phi(:,1));
filt.P0         = zeros(filt.x_num,filt.x_num);

% Filter states idx
filt.eul_idx        = uint8(1:3);
filt.g_idx          = uint8(4);
filt.rot_bias_idx   = uint8(5:7);
filt.acc_bias_idx   = uint8(8:10);
filt.lin_acc_idx    = uint8(11:13);

filt.rot_noise_idx  = uint8(1:3);

% Transform doubles to single
filt            = double2single(filt);