
var gl;
var points;
var canvas;

window.onload = function init()
{
    canvas = document.getElementById( "gl-canvas" );
    
    gl = WebGLUtils.setupWebGL( canvas );
    if ( !gl ) { alert( "WebGL isn't available" ); }

    
    var xhr;
    if (window.XMLHttpRequest) {
        xhr = new XMLHttpRequest();
    } else if (window.ActiveXObject) {
        xhr = new ActiveXObject("Microsoft.XMLHTTP");
    }

    xhr.onreadystatechange = function(){
        if(xhr.readyState === XMLHttpRequest.DONE && xhr.status === 200){
            drawTruss(xhr.responseText);
        }
    };
    xhr.open("GET","/data/example.json"); 
    xhr.send();

};

function drawTruss(raw){

    var data = JSON.parse(raw);

    var vertices = [];
    var edges = []; 

    for(var i=0;i<data.Vertices.length;i++){
    	vertices.push(vec3(data.Vertices[i].XYZPosition));
    }

    for(var i=0;i<data.Edges.length;i++){
        edges.push(data.Edges[i].Endpoints);
    }

    vertices = normalize(vertices);

    //
    //  Configure WebGL
    //
    gl.viewport( 0, 0, canvas.width, canvas.height );
    gl.clearColor( 0.0, 0.0, 0.0, 1.0 );
    
    //  Load shaders and initialize attribute buffers
    
    var program = initShaders( gl, "vertex-shader", "fragment-shader" );
    gl.useProgram( program );
    
    // Load the data into the GPU
    
    var bufferId = gl.createBuffer();
    gl.bindBuffer( gl.ARRAY_BUFFER, bufferId );
    gl.bufferData( gl.ARRAY_BUFFER, flatten(vertices), gl.STATIC_DRAW );

    edgeIndexBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, edgeIndexBuffer);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(flatten(edges)), gl.STATIC_DRAW);
	

    // Associate out shader variables with our data buffer
    
    var vPosition = gl.getAttribLocation( program, "vPosition" );
    gl.vertexAttribPointer( vPosition, 3, gl.FLOAT, false, 0, 0 );
    gl.enableVertexAttribArray( vPosition );

    gl.clear( gl.COLOR_BUFFER_BIT );

    gl.bindBuffer( gl.ARRAY_BUFFER, bufferId );
    gl.drawArrays( gl.POINTS, 0, vertices.length);
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, edgeIndexBuffer);
    gl.drawElements(gl.LINES, edges.length*2, gl.UNSIGNED_SHORT, 0)
}

function normalize(data){
    var mag=0.00;
    var maxX, minX, maxY, minY, maxZ, minZ;
    maxX = minX = maxY = minY = maxZ = minZ = 0;
    for(i=0; i<data.length; i++){
        if(data[i][0]>maxX) maxX=data[i][0];
        if(data[i][0]<minX) minX=data[i][0];
        if(data[i][1]>maxY) maxY=data[i][1];
        if(data[i][1]<minY) minY=data[i][1];
        if(data[i][2]>maxZ) maxZ=data[i][2];
        if(data[i][2]<minZ) minZ=data[i][2];
    }
    mag=maxX-minX;
    if(mag < maxY-minY) mag = maxY-minY;
    if(mag < maxZ-minZ) mag = maxZ-minZ;
    mag=mag/1.5;
    var dx=-(maxX+minX)/(2*mag);
    var dy=-(maxY+minY)/(2*mag);
    var dz=-(maxZ+minZ)/(2*mag);

    var out = [];
    var x, y, z;
    for(i=0; i<data.length; i++){
        x=data[i][0]/mag+dx;
        y=data[i][1]/mag+dy;
        z=data[i][2]/mag+dz;
        out.push([x, y, z])
    }
    return out;
}