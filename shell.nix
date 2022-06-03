with import <nixpkgs> {};
stdenv.mkDerivation {
  name = "env";
  nativeBuildInputs = [ cmake boost-build ];
  buildInputs = [
    boost
  ];
}
