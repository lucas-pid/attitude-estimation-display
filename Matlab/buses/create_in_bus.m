function inp_Bus = create_in_bus()

% Define struct with hyerarchy and size of the bus elements
inp_bus_struct.sfor_meas_B_mDs2             = zeros(3,1,'single');
inp_bus_struct.rot_K_meas_IB_B_radDs        = zeros(3,1,'single');
inp_bus_struct.stop_correction_bool         = false;
inp_bus_struct.mode_idx                     = int8(0);
inp_bus_struct.delta_x0                     = zeros(13,1,'single');
inp_bus_struct.reset_bool                   = false;

% Create bus
inp_Bus = struct2bus(inp_bus_struct);

% Set a separate header file to export the bus and use it with all the
% filters
inp_Bus.HeaderFile  = 'buses_definition.h';
inp_Bus.DataScope   = 'Exported';
end