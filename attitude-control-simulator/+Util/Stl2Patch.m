function varargout = Stl2Patch(file)
    fid = fopen(file,'r'); rawStlFile = fread(fid,inf,'uint8=>uint8'); fclose(fid);

%fid is pointer to 'file', rawStlFile is made of fid, what does inf and uin8 do?

    numTriangles = typecast(rawStlFile(81:84),'uint32');

%what is typecast?

    triangleData = rawStlFile(85:end);
    [faces, vetices] = deal(zeros(numTriangles,3), zeros(3*numTriangles,3));
    i = 0;
    while i < numTriangles
        frame_begin = 50*i+1; frame_end = frame_begin+50-1;
        triangleFrame = triangleData(frame_begin:frame_end)';
        v = double([typecast(triangleFrame(13:24),'single');
                    typecast(triangleFrame(25:36),'single');
                    typecast(triangleFrame(37:48),'single')]);
        f_i  = i+1; v_begin = 3*(f_i-1)+1; v_end = v_begin+3-1;
        vertices(v_begin:v_end,:) = v; faces(f_i,:) = v_begin:v_end;
        i = i+1;
    end
    varargout = {struct('faces',faces,'vertices',vertices)};
end
