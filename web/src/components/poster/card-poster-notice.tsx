// "use client";

import {
  Card,
  Table,
  TableHeader,
  TableColumn,
  TableBody,
  TableRow,
  TableCell,
  Divider,
} from "@nextui-org/react";

export default function CardPosterNotice() {
  return (
    <Card className="grow">
      <div className="mx-10 mt-5 mb-5 flex gap-x-5 justify-end items-center">
        <p className="grow mr-5 text-3xl font-serif font-bold text-center">
          Notice
        </p>
      </div>
      <div className="mx-5 mt-2">
        <p className="text-center">
          UnitCamS3 will try to connect the wifi and start posting images.
        </p>

        <Divider className="mt-5 mb-10"></Divider>

        <p className="my-5 text-center">
          The <span className="font-bold">LED Status</span> shows the current
          state:
        </p>
        <Table aria-label="Mode Table" className="my-5">
          <TableHeader>
            <TableColumn>LED STATUS</TableColumn>
            <TableColumn>STATE</TableColumn>
          </TableHeader>
          <TableBody>
            <TableRow key="1">
              <TableCell>Breathing</TableCell>
              <TableCell>Connecting WiFi</TableCell>
            </TableRow>
            <TableRow key="2">
              <TableCell>Fast Blinking</TableCell>
              <TableCell>Posting image</TableCell>
            </TableRow>
            <TableRow key="3">
              <TableCell>Slow Blinking</TableCell>
              <TableCell>Wait next post</TableCell>
            </TableRow>
            <TableRow key="4">
              <TableCell>Constantly on</TableCell>
              <TableCell>
                Wait config <span className="font-light">(default)</span>
              </TableCell>
            </TableRow>
          </TableBody>
        </Table>

        <Divider className="mt-10 mb-5"></Divider>

        <p className="grow mr-5 text-3xl font-serif font-bold text-center">
          Reset
        </p>

        <p className="mt-5 mb-5 text-center">
          If the configed WiFi is valid, there will be 10 seconds of waiting on
          every reboot, before starting image poster.
        </p>
        <p className="mt-5 mb-5 text-center">
          You can connect to <span className="font-bold">UnitCamS3-WiFi</span>{" "}
          and reconfigure your settings.
        </p>
      </div>
    </Card>
  );
}
