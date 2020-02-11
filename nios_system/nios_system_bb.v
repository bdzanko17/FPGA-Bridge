
module nios_system (
	clk_clk,
	reset_reset_n,
	tse1_mac_mdio_connection_mdc,
	tse1_mac_mdio_connection_mdio_in,
	tse1_mac_mdio_connection_mdio_out,
	tse1_mac_mdio_connection_mdio_oen,
	tse1_mac_rgmii_connection_rgmii_in,
	tse1_mac_rgmii_connection_rgmii_out,
	tse1_mac_rgmii_connection_rx_control,
	tse1_mac_rgmii_connection_tx_control,
	tse1_mac_status_connection_set_10,
	tse1_mac_status_connection_set_1000,
	tse1_mac_status_connection_eth_mode,
	tse1_mac_status_connection_ena_10,
	tse1_pcs_mac_rx_clock_connection_clk,
	tse1_pcs_mac_tx_clock_connection_clk,
	tse_mac_mdio_connection_mdc,
	tse_mac_mdio_connection_mdio_in,
	tse_mac_mdio_connection_mdio_out,
	tse_mac_mdio_connection_mdio_oen,
	tse_mac_rgmii_connection_rgmii_in,
	tse_mac_rgmii_connection_rgmii_out,
	tse_mac_rgmii_connection_rx_control,
	tse_mac_rgmii_connection_tx_control,
	tse_mac_status_connection_set_10,
	tse_mac_status_connection_set_1000,
	tse_mac_status_connection_eth_mode,
	tse_mac_status_connection_ena_10,
	tse_pcs_mac_rx_clock_connection_clk,
	tse_pcs_mac_tx_clock_connection_clk);	

	input		clk_clk;
	input		reset_reset_n;
	output		tse1_mac_mdio_connection_mdc;
	input		tse1_mac_mdio_connection_mdio_in;
	output		tse1_mac_mdio_connection_mdio_out;
	output		tse1_mac_mdio_connection_mdio_oen;
	input	[3:0]	tse1_mac_rgmii_connection_rgmii_in;
	output	[3:0]	tse1_mac_rgmii_connection_rgmii_out;
	input		tse1_mac_rgmii_connection_rx_control;
	output		tse1_mac_rgmii_connection_tx_control;
	input		tse1_mac_status_connection_set_10;
	input		tse1_mac_status_connection_set_1000;
	output		tse1_mac_status_connection_eth_mode;
	output		tse1_mac_status_connection_ena_10;
	input		tse1_pcs_mac_rx_clock_connection_clk;
	input		tse1_pcs_mac_tx_clock_connection_clk;
	output		tse_mac_mdio_connection_mdc;
	input		tse_mac_mdio_connection_mdio_in;
	output		tse_mac_mdio_connection_mdio_out;
	output		tse_mac_mdio_connection_mdio_oen;
	input	[3:0]	tse_mac_rgmii_connection_rgmii_in;
	output	[3:0]	tse_mac_rgmii_connection_rgmii_out;
	input		tse_mac_rgmii_connection_rx_control;
	output		tse_mac_rgmii_connection_tx_control;
	input		tse_mac_status_connection_set_10;
	input		tse_mac_status_connection_set_1000;
	output		tse_mac_status_connection_eth_mode;
	output		tse_mac_status_connection_ena_10;
	input		tse_pcs_mac_rx_clock_connection_clk;
	input		tse_pcs_mac_tx_clock_connection_clk;
endmodule
