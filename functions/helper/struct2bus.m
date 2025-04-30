function in_struct_Bus = struct2bus(in_struct)
% Outputs bus object given a "one level" struct (does not work for nested struct!!)

% Get fields of struct
field_names = fieldnames(in_struct);

% Number of fields
N = numel(field_names);

% Initialize bus elements

% Loop over fields
for ii = 1:N

    % Get name of field ii
    name_ii = field_names{ii};

    % Get the size of element ii
    sz = size(in_struct.(name_ii));

    % Get the data type
    data_type = class(in_struct.(name_ii)(1));
    
    % If logical change to boolean....
    if islogical(in_struct.(name_ii)(1))
        data_type = 'boolean';
    end

    % Create element object
    elements(ii) = Simulink.BusElement;

    % Assign properties
    elements(ii).DataType    = data_type;
    elements(ii).Name        = name_ii;
    elements(ii).Dimensions  = sz;
end

% Define output bus
in_struct_Bus = Simulink.Bus;
in_struct_Bus.Elements = elements;