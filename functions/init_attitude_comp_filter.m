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

% Transform doubles to single
filt = double2single(filt);