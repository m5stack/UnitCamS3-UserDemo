// "use client";

import TitleBar from "../components/title-bar";
import CardShooterConfig from "../components/shooter/card-shooter-config";

export default function PageShooter() {
  return (
    <div className="flex flex-col">
      <TitleBar></TitleBar>

      <div className="flex flex-col mt-24 mb-8 px-2 gap-8 sm:w-3/4 sm:mx-auto">
        <CardShooterConfig></CardShooterConfig>
      </div>
    </div>
  );
}
