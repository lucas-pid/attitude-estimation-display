function in_struct = double2single(in_struct)

field_names = fieldnames(in_struct);

for ii = 1:numel(field_names)


    val = in_struct.(field_names{ii});

    if isa(val, 'double')
        in_struct.(field_names{ii}) = single(val);
    end
end
end