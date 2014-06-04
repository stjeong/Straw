name := "StrawServer"

version := "1.0-SNAPSHOT"

libraryDependencies ++= Seq(
  javaJdbc,
  javaEbean,
  cache
)

libraryDependencies += "org.mongodb" % "mongo-java-driver" % "2.12.2"

play.Project.playJavaSettings
