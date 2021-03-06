package org.dolphinemu.dolphinemu;

import android.content.Context;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class NativeGLSurfaceView extends SurfaceView {
	static private String FileName;
	static private Thread myRun;
	static private boolean Running = false;
	static private boolean Created = false;
	static private float width;
	static private float height;

	public NativeGLSurfaceView(Context context) {
		super(context);
		if (!Created)
		{
			myRun = new Thread() 
			{
				@Override
	      		public void run() {
	    	  		NativeLibrary.Run(FileName, getHolder().getSurface(), (int)width, (int)height);
	      		}	
			};
			getHolder().addCallback(new SurfaceHolder.Callback() {
		            public void surfaceCreated(SurfaceHolder holder) {
		                // TODO Auto-generated method stub
		            	if (!Running)
		            	{
		            		myRun.start();
		            		Running = true;
		            	}
		            }

					public void surfaceChanged(SurfaceHolder arg0, int arg1,
							int arg2, int arg3) {
						// TODO Auto-generated method stub
						
					}

					public void surfaceDestroyed(SurfaceHolder arg0) {
						// TODO Auto-generated method stub
						
					}
			 });
			Created = true;
		}
	}
	
	public void SetFileName(String file)
	{
		FileName = file;
	}
	
	public void SetDimensions(float screenWidth, float screenHeight)
	{
		width = screenWidth;
		height = screenHeight;
	}
}
