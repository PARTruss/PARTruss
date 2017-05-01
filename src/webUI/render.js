
var gl;
var points;
var canvas;

var submit, url, submit2, file;

var camera = {
    near: 0.01,
    far: 100,
    aspect: 0.7,
    fovAngle: 90,
    z: 4.0
}

var dx=0,dy=0,dz=0;

var data;

var modelViewMatrixLoc, projectionMatrixLoc;

var nodeprogram, edgeprogram;

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
    /*
    submit2=document.getElementById( "submit2" );
    file = document.getElementById( "file" );
    */
    submit.onclick=getAndDrawUrl;
    /*
    submit2.onclick=function(){
        getAndDrawFile(this.files[0]);
    }
    */

    var shift = false;

    var lastx=0, lasty=0, lastz=0, lastTheta=0, lastPhi=0;

    canvas.onmousedown = function(e) {
        isDown = true;
        shift=e.shiftKey;

        startCoords = [
            e.offsetX , // set start coordinates
            e.offsetY
       ];
    };

    canvas.onmouseup   = function(e) {
        isDown = false;
        shift=false;
        last = [
            e.offsetX - startCoords[0], // set last coordinates
            e.offsetY - startCoords[1]
        ];

        lasxx=dx;
        lasty=dy;
        lastz=dz;
        lastTheta=theta;
        lastPhi=phi;

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
        var xDiff=(x-startCoords[0])/100;
        var yDiff=(y-startCoords[1])/100;
        if(e.shiftKey){
            dx=lastx-xDiff*Math.cos(theta) - yDiff*Math.sin(theta)*Math.sin(phi);
            dy=lasty+yDiff*Math.cos(phi);
            dz=lastz+xDiff*Math.sin(theta) - yDiff*Math.cos(theta)*Math.sin(phi);
        }
        else{
            theta=lastTheta-xDiff;
            phi=lastPhi+yDiff;
        }
        render(); // render to show changes

    }

    var panning=false;

    document.addEventListener('wheel', function(event)
    {
        event.preventDefault();
        if (event.deltaY > 0) camera.z += 0.1;
        else camera.z -= 0.1;
        if(camera.z<0.1) camera.z=0.1;  
        render();  
    })

    getAndDrawUrl();

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

    data = JSON.parse(raw);

    vertices = [];
    edges = []; 

    for(var i=0;i<data.Vertices.length;i++){
    	vertices.push(vec3(data.Vertices[i].XYZPosition));
    }

    for(var i=0;i<data.Edges.length;i++){
        edges.push(data.Edges[i].Endpoints);
    }

    camera.z=zoomToFit(vertices);

    gl.viewport( 0, 0, canvas.width, canvas.height );
    gl.clearColor( 0.0, 0.0, 0.0, 1.0 );

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
    }

    modelViewMatrix = lookAt( eye, at, up );
    projectionMatrix = perspective(camera.fovAngle, camera.aspect, camera.near, camera.far);

    gl.clear( gl.COLOR_BUFFER_BIT );
    renderEdges();
    renderNodes();
    

}

function renderNodes(){
    nodeprogram = initShaders( gl, "node-vertex-shader", "node-fragment-shader" );
    gl.useProgram( nodeprogram );

    modelViewMatrixLoc = gl.getUniformLocation( nodeprogram, "modelViewMatrix" );
    projectionMatrixLoc = gl.getUniformLocation( nodeprogram, "projectionMatrix" );

    gl.uniformMatrix4fv( modelViewMatrixLoc, false, flatten(modelViewMatrix) );
    gl.uniformMatrix4fv( projectionMatrixLoc, false, flatten(projectionMatrix) );
        // Load the data into the GPU
    
    bufferId = gl.createBuffer();
    gl.bindBuffer( gl.ARRAY_BUFFER, bufferId );
    gl.bufferData( gl.ARRAY_BUFFER, flatten(vertices), gl.STATIC_DRAW );

    var vPosition = gl.getAttribLocation( nodeprogram, "vPosition" );
    gl.vertexAttribPointer( vPosition, 3, gl.FLOAT, false, 0, 0 );
    gl.enableVertexAttribArray( vPosition );

    gl.bindBuffer( gl.ARRAY_BUFFER, bufferId );
    gl.drawArrays( gl.POINTS, 0, vertices.length);

}

function renderEdges(){
    edgeprogram = initShaders( gl, "edge-vertex-shader", "edge-fragment-shader" );
    gl.useProgram( edgeprogram );

    modelViewMatrixLoc = gl.getUniformLocation( edgeprogram, "modelViewMatrix" );
    projectionMatrixLoc = gl.getUniformLocation( edgeprogram, "projectionMatrix" );
    
    gl.uniformMatrix4fv( modelViewMatrixLoc, false, flatten(modelViewMatrix) );
    gl.uniformMatrix4fv( projectionMatrixLoc, false, flatten(projectionMatrix) );

    var points = [];

    var ver = [];

    for(var i=0;i<data.Vertices.length;i++){
        ver.push(vec3(data.Vertices[i].XYZPosition));
    }

    for(var i=0;i<data.Edges.length;i++){
        points.push(vec4(ver[data.Edges[i].Endpoints[0]],data.Edges[i].Force));
        points.push(vec4(ver[data.Edges[i].Endpoints[1]],data.Edges[i].Force));
        
    }
    console.log(flatten(points));

    var edgebufferId = gl.createBuffer();
    gl.bindBuffer( gl.ARRAY_BUFFER, edgebufferId );
    gl.bufferData( gl.ARRAY_BUFFER, flatten(points), gl.STATIC_DRAW );
    
    var vPosition = gl.getAttribLocation( edgeprogram, "vPosition" );
    gl.vertexAttribPointer( vPosition, 3, gl.FLOAT, false, 16, 0 );
    var hue = gl.getAttribLocation( edgeprogram, "hue" );
    gl.vertexAttribPointer( hue, 1, gl.FLOAT, false, 16, 12 );
    gl.enableVertexAttribArray( vPosition );
    gl.enableVertexAttribArray( hue );

    gl.bindBuffer( gl.ARRAY_BUFFER, edgebufferId );
    gl.drawArrays( gl.LINES, 0, data.Edges.length*2);

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

function zoomToFit(data){
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
    mag=mag;
    return mag;
}