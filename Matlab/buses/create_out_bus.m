function out_Bus = create_out_bus()

% Define struct with hyerarchy and size of the bus elements
out_bus_struct.Eul_rad                      = zeros(3,1,'single');
out_bus_struct.rot_K_est_OB_B_radDs         = zeros(3,1,'single');
out_bus_struct.sfor_est_B_mDs               = zeros(3,1,'single');
out_bus_struct.rot_K_est_bias_OB_B_radDs    = zeros(3,1,'single');
out_bus_struct.sfor_est_bias_B_mDs2         = zeros(3,1,'single');
out_bus_struct.g_est_mDs2                   = zeros(1,1,'single');
out_bus_struct.lin_acc_est_mDs2             = zeros(3,1,'single');
out_bus_struct.x_est                        = zeros(13,1,'single');
out_bus_struct.mode                         = uint8(0);

% Create bus
out_Bus = struct2bus(out_bus_struct);

% Set a separate header file to export the bus and use it with all the
% filters
out_Bus.HeaderFile  = 'buses_definition.h';
out_Bus.DataScope   = 'Exported';
end