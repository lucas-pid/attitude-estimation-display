clc; clear

%% Initialize model parameters
sim.sample_time_s = 1/100;

% Load buses
load_buses;

% Load model configuration
configuration_filter = filter_configuration();

%% Define filter parameters
filt.sample_time_s  = 1/100;
filt.omega_radDs    = 1;        % Complementary filter frequency
filt.rot_bias_radDs = [0;0;0];
filt.sfor_bias_mDs2 = [0;0;0];

% Filter states idx
filt.eul_idx = uint8(1:3);
filt.g_idx = uint8(4);
filt.rot_bias_idx = uint8(5:7);
filt.acc_bias_idx = uint8(8:10);
filt.rot_noise_idx = uint8(1:3);

% Transform doubles to single
filt = double2single(filt);