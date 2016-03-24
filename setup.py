from distutils.core import setup, Extension

curvemath = Extension("fastecdsa.curvemath",
                      include_dirs=['src/'],
                      libraries=['gmp'],
                      sources=['src/curveMath.c', 'src/curve.c', 'src/point.c'])

setup(name='fastecdsa',
      version='0.1',
      description='Fast elliptic curve arithmetic',
      packages=['fastecdsa'],
      ext_modules=[curvemath])
