
var gl;
var points;
var canvas;

var submit, url, submit2, file;

var camera = {
    near: 0.1,
    far: 5,
    aspect: 0.7,
    fovAngle: 90,
    z: 2.0
}

var dx=0,dy=0,dz=0;

var modelViewMatrixLoc, projectionMatrixLoc;

var radius = 6.0;

var theta=0.0;
var phi=0.0;
var dr = 0.5 * Math.PI/180.0;

var bufferId, edgeIndexBuffer;
var vertices, edges;

var isDown = false; // whether mouse is pressed
var startCoords = []; // 'grab' coordinates when pressing mouse
var last = [0, 0]; // previous coordinates of mouse release

window.onload = function init()
{
    canvas = document.getElementById( "gl-canvas" );
    
    gl = WebGLUtils.setupWebGL( canvas );
    if ( !gl ) { alert( "WebGL isn't available" ); }

    
    submit=document.getElementById( "submit" );
    url = document.getElementById( "url" );
    submit2=document.getElementById( "submit2" );
    file = document.getElementById( "file" );
    submit.onclick=getAndDrawUrl;
    submit2.onclick=function(){
        getAndDrawFile(this.files[0]);
    }

    var shift = false;
    canvas.onmousedown = function(e) {
        isDown = true;
        shift=e.shiftKey;

        startCoords = [
            e.offsetX - last[0], // set start coordinates
            e.offsetY - last[1]
       ];
    };

    canvas.onmouseup   = function(e) {
        isDown = false;
        shift=false;
        last = [
            e.offsetX - startCoords[0], // set last coordinates
            e.offsetY - startCoords[1]
        ];
    };

    canvas.onmousemove = function(e)
    {
        if(!isDown) return; // don't pan if mouse is not pressed
        if(shift!=e.shiftKey){
            canvas.onmouseup(e);
            canvas.onmousedown(e);
        }
        var x = e.offsetX;
        var y = e.offsetY;
        var xDiff=(x - startCoords[0])/100;
        var yDiff=(y - startCoords[1])/100;
        if(e.shiftKey){
            dx=-xDiff*Math.cos(theta) - yDiff*Math.sin(theta)*Math.sin(phi);;
            dy=yDiff*Math.cos(phi);
            dz=xDiff*Math.sin(theta) - yDiff*Math.cos(theta)*Math.sin(phi);
        }
        else{
            theta=-xDiff;
            phi=yDiff;
        }
        console.log(e.shiftKey);
        render(); // render to show changes

    }

    var panning=false;

    document.addEventListener('mousewheel', function(event)
    {
        event.preventDefault();
        if (event.deltaY > 0) camera.z += 0.1;
        else camera.z -= 0.1;      
        render();  
    })


};

function getAndDrawFile(f){

    var reader = new FileReader();

    // inject an image with the src url
    reader.onload = function(event) {
        drawTruss(event.target.result);
    }
 
    // when the file is read it triggers the onload event above.
    reader.readAsDataURL(f);
}

function getAndDrawUrl(){
    
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
    xhr.open("GET",url.value); 
    xhr.send();
}

function drawTruss(raw){

    var data = JSON.parse(raw);

    vertices = [];
    edges = []; 

    for(var i=0;i<data.Vertices.length;i++){
    	vertices.push(vec3(data.Vertices[i].XYZPosition));
    }

    for(var i=0;i<data.Edges.length;i++){
        edges.push(data.Edges[i].Endpoints);
    }

    vertices = normalizeShape(vertices);

    //
    //  Configure WebGL
    //
    gl.viewport( 0, 0, canvas.width, canvas.height );
    gl.clearColor( 0.0, 0.0, 0.0, 1.0 );
    
    //  Load shaders and initialize attribute buffers
    
    var program = initShaders( gl, "vertex-shader", "fragment-shader" );
    gl.useProgram( program );

    modelViewMatrixLoc = gl.getUniformLocation( program, "modelViewMatrix" );
    projectionMatrixLoc = gl.getUniformLocation( program, "projectionMatrix" );

        // Load the data into the GPU
    
    bufferId = gl.createBuffer();
    gl.bindBuffer( gl.ARRAY_BUFFER, bufferId );
    gl.bufferData( gl.ARRAY_BUFFER, flatten(vertices), gl.STATIC_DRAW );

    edgeIndexBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, edgeIndexBuffer);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(flatten(edges)), gl.STATIC_DRAW);

    // Associate out shader variables with our data buffer
    
    var vPosition = gl.getAttribLocation( program, "vPosition" );
    gl.vertexAttribPointer( vPosition, 3, gl.FLOAT, false, 0, 0 );
    gl.enableVertexAttribArray( vPosition );

    render();
}

function render(){
    var eye = [ Math.sin(theta)*Math.cos(phi)*camera.z + dx,
                Math.sin(phi)*camera.z + dy, 
                Math.cos(theta)*Math.cos(phi)*camera.z + dz] ;
    var at = [dx, dy, dz];

    up = vec3(0.0, 1.0, 0.0);

    if(Math.cos(phi)<0) {
        up[1]=-1;
        console.log("inverse up");
    }

    modelViewMatrix = lookAt( eye, at, up );
    projectionMatrix = perspective(camera.fovAngle, camera.aspect, camera.near, camera.far);

    gl.uniformMatrix4fv( modelViewMatrixLoc, false, flatten(modelViewMatrix) );
    gl.uniformMatrix4fv( projectionMatrixLoc, false, flatten(projectionMatrix) );

    

    gl.clear( gl.COLOR_BUFFER_BIT );

    gl.bindBuffer( gl.ARRAY_BUFFER, bufferId );
    gl.drawArrays( gl.POINTS, 0, vertices.length);
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, edgeIndexBuffer);
    gl.drawElements(gl.LINES, edges.length*2, gl.UNSIGNED_SHORT, 0)
}

function normalizeShape(data){
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