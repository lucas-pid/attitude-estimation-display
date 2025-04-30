function [Mx, My, Mz] = get_rot_matrix(phi, theta, psi)
% Outputs rotation matrices given the euler angles    
Mx = [1,0,0; 0, cos(phi), sin(phi); 0, -sin(phi), cos(phi)];
My = [cos(theta), 0, -sin(theta); 0, 1, 0; sin(theta), 0, cos(theta)];
Mz = [cos(psi), sin(psi), 0; -sin(psi), cos(psi), 0; 0, 0, 1];
end