import { Card, Divider } from "@nextui-org/react";
import TitleBar from "../components/title-bar";
import CardCameraTest from "../components/hardware-test/card-camera-test";
import CardSdCardTest from "../components/hardware-test/card-sd-card-test";
import CardMicTest from "../components/hardware-test/card-mic-test";
import CardLedTest from "../components/hardware-test/card-led-test";
import CardPoster from "../components/poster/card-poster";
import CardShooter from "../components/shooter/card-shooter";

function PageRoot() {
  return (
    <div className="flex flex-col">
      <TitleBar></TitleBar>

      <div className="flex flex-col mt-24 mb-8 px-2 gap-8 sm:w-3/4 sm:mx-auto">
        <Card className="grow">
          <CardCameraTest></CardCameraTest>

          <Divider></Divider>

          <CardSdCardTest></CardSdCardTest>

          <Divider></Divider>

          <CardMicTest></CardMicTest>

          <Divider></Divider>

          <CardLedTest></CardLedTest>
        </Card>

        <Card className="grow">
          <CardPoster></CardPoster>
        </Card>

        <Card className="grow">
          <CardShooter></CardShooter>
        </Card>
      </div>
    </div>
  );
}

export default PageRoot;
