// "use client";

import TitleBar from "../components/title-bar";
import CardShooterStart from "../components/shooter/card-shooter-start";

export default function PageShooterStart() {
  return (
    <div className="flex flex-col">
      <TitleBar></TitleBar>

      <div className="flex flex-col mt-24 mb-8 px-2 gap-8 sm:w-3/4 sm:mx-auto">
        <CardShooterStart></CardShooterStart>
      </div>
    </div>
  );
}
