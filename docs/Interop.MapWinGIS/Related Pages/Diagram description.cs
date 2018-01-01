#pragma warning disable 1587
/// \page diagrams Diagram description
/// 
/// \section one I. Pseudo class diagram.
/// This diagram represents "contains" type of the relation between 2 classes, i.e. when an instance of the class
/// holds some of instances of the other classes and expose access to them through certain properties. For example
/// an instance of Shapefile class may hold several instance of Shape class (1 to 0..n relation), an instance of 
/// Shape class in the same manner may hold points. \n\n
/// The diagrams are interactive. After the click on the rectangle
/// the description of corresponding class will be shown, after the click on the arrow - a property which extracts
/// instances of nested class. These diagrams has nothing to do with the inheritance.
/// \dot
/// digraph legend {
/// nodesep = 0.3;
/// ranksep = 0.2;
/// splines = ortho;
/// node [shape= "polygon", peripheries = 2, fontname=Helvetica, fontsize=9, color = gray, style = filled, height = 0.3, width = 0.8];
/// coll [ label="Holds collection\n of instances of class D"];
/// node [color = tan, peripheries = 1];
/// class [ label="The instance\n of CLASS A"];
/// child [ label="Holds instance \nof class C"];
/// node [style = dashed, color = gray];
/// prnt [ label= "Can be stored in\ instance of class B"];
/// edge [ arrowhead="open", style = dashed, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" ]
/// prnt -> class;
/// edge [style = solid]
/// class -> child;
/// class -> coll;
/// }
/// \enddot
/// \section two II. Groups of API members.
/// The description of some large classes is separated into several pages, which are called modules. This type of diagram is used for faster navigation
/// between such groups. In case the methods and properties of the module were deprecated the gray color will be used.
/// \dot
/// digraph groups_leg {
/// splines = true;
/// ranksep = 0.2;
/// nodesep = 0.3;
/// node [shape= "polygon", peripheries = 1, fontname=Helvetica, fontsize=9, color = palegreen, style = filled, height = 0.3, width = 0.8]
/// cl [ label="The class"];
/// node  [shape = "ellipse", color = khaki, width = 0.2, height = 0.2 ]
/// gr1 [label="Holds group\n of members A"];
/// gr2 [label="Holds group\n of members B"];
/// node [color = gray]
/// gr3 [label="Holds deprecated\n group C"];
/// edge [ arrowhead="open", style = solid, arrowsize = 0.6, fontname = "Arial", fontsize = 9, fontcolor = blue, color = "#606060" ]
///   cl -> gr1;
///   cl -> gr2;
///   cl -> gr3;
/// }
/// \enddot
#pragma warning restore 1587