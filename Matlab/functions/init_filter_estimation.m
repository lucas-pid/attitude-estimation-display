% Initializes parameters for the kalman and complementary filters
clc; clear

%% Model parameters
sim.sample_time_s = 1/100;

% Load buses
load_buses;

% Load model configuration
configuration_filter = filter_configuration();

%% Kalman Filter parameters
% x_dot = A*x + B*u + B_w*w
%     z = C*x + v

% A matrix
% Jacobian at phi=theta=0. Some elements will change accordingly to the
% attitude angles
% States:   Euler,              g,          rot_bias,       acc_bias,       linear_acceleration
A = [       zeros(3,3),     zeros(3,1),     -eye(3,3),      zeros(3,3),         zeros(3,3)
            zeros(7,13)
            zeros(3,10),                                                        -eye(3)*1    ];

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

% Observation matrix jacobian at phi=theta=0
C = [M_B_O_dphi*g0, M_B_O_dtheta*g0, M_B_O_dpsi*g0, g0, zeros(3,3), eye(3), eye(3)];

% Disturbances B matrix
% Disturbances: gyro noise,     Rot bias noise,     acc bias noise,     linear acceleration noise 
B_w     = [     eye(3),         zeros(3,9)                                                      % Euler
                zeros(1,12)                                                                     % g
                zeros(3,3),     eye(3),             zeros(3,6)                                  % Rot bias
                zeros(3,3),     zeros(3,3),         eye(3),             zeros(3,3)              % Acc bias
                zeros(3,9),                                             eye(3)];                % Linear acceleration

% Zero psi_dot contribution
% Since there is no magnetometer, the heading (psi) cannot be corrected
B_w(3,3) = 0;
B_w(7,6) = 0;

% Process covariance
rot_var         = (0.03 * pi/180)^2 * 100 * ones(3,1);      % From sensor spreadsheet
meas_acc_var    = (0.22*1e-3 * 9.81)^2 * 100 * ones(3,1);   % From sensor spreadsheet
lin_acc_var     =  5e-2 * ones(3,1);
rot_bias_var    = 1e-10 * ones(3,1);
acc_bias_var    = 1e-10 * ones(3,1);

% Assemble process covariance matrix
Q = diag([rot_var; rot_bias_var; acc_bias_var; lin_acc_var]);

% Measurement covariance matrix
R = diag(meas_acc_var);

% Assemble filter struct with all matrices
filt.sample_time_s  = 1/100;
filt.Phi            = expm(A * filt.sample_time_s); % Transformation to discrete (should be similar to I + A*Ts)
filt.Gamma_u        = B   * filt.sample_time_s;     % Simplified transformation to discrete
filt.Gamma_w        = B_w * filt.sample_time_s;     % Simplified transformation to discrete
filt.Q              = Q;
filt.R              = R;
bias_gyro           = [0; 0; 0];
bias_acc            = [0; 0; 0];
g                   = -9.81;
filt.x0             = [deg2rad([0; 0; 0]); g; bias_gyro; bias_acc; zeros(3,1)];
filt.C              = C;
filt.I_n            = eye(length(A(:,1)));
filt.x_num          = length(filt.Phi(:,1));
filt.P0             = zeros(filt.x_num,filt.x_num);

% Filter states idx
filt.eul_idx        = uint8(1:3);
filt.g_idx          = uint8(4);
filt.rot_bias_idx   = uint8(5:7);
filt.acc_bias_idx   = uint8(8:10);
filt.lin_acc_idx    = uint8(11:13);
filt.rot_noise_idx  = uint8(1:3);

%% Complementary filter parameters
filt.omega_radDs    = 1;            % Complementary filter frequency

%% Operating modes
filt.modes.kalman_filter    = uint8(0);
filt.modes.dead_reckoning   = uint8(1);
filt.modes.comp_filter      = uint8(2);
filt.modes.inclinometer     = uint8(3);

%% Sensor alignment time constant
filt.alignment_tau_s = 1;

%% Transform double precision parameters to single
filt = double2single(filt);