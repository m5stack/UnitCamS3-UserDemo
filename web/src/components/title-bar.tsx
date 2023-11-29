// "use client";

// import { Link } from "@nextui-org/react";

export default function TitleBar() {
//   const docLink: string = "https://www.github.com";
//   const githubLink: string = "https://www.github.com";

  return (
    <div className="fixed w-full h-16 z-50 bg-white/50 backdrop-blur-lg flex shadow">
      <div className="w-full ml-5 mt-1.5 flex flex-col">
        <p className="font-bold font-serif text-xl">UNIT-CAM-S3</p>

        <div className="flex ml-px mr-5">
          <p className="font-serif font-light">User Demo</p>

          <div className="grow"></div>

          {/* <Link
            href={docLink}
            underline="hover"
            color="foreground"
            className="mr-5 font-serif font-light"
          >
            Document
          </Link>

          <Link
            href={githubLink}
            underline="hover"
            color="foreground"
            className="font-serif font-light"
          >
            Github
          </Link> */}

        </div>
      </div>
    </div>
  );
}
