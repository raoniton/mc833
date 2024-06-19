from scapy.all import rdpcap, IP, ICMP
from datetime import datetime

def extract_icmp_info(pcap_file):
    packets = rdpcap(pcap_file)
    icmp_packets = [pkt for pkt in packets if ICMP in pkt]
    
    if not icmp_packets:
        print(f"Nenhum ICMP packet encontrado no arquivo -> '{pcap_file}'")
        return
    
    src_ips = []
    dst_ips = []
    packet_times = []
    tuplasDestSrc = set()
    total_size = 0

    for pkt in icmp_packets:
        if IP in pkt and ICMP in pkt:
            #src_ips.append(pkt[IP].src)
            #dst_ips.append(pkt[IP].dst)
            tupla = (pkt[IP].src ,  pkt[IP].dst)
            if tupla not in tuplasDestSrc:
                tuplasDestSrc.add(tupla)
            packet_times.append(pkt.time)
            total_size += len(pkt)
    
    # Calculate statistics
    total_packets = len(icmp_packets)
    total_time = packet_times[-1] - packet_times[0] if total_packets > 1 else 0
    average_throughput = (total_size / total_time) if total_time > 0 else 0
    average_interval = (total_time / (total_packets - 1)) if total_packets > 1 else 0

    print("Analise de packets sob o protocolo ICMP")
    #print(f"Source IPs: {set(src_ips)}")
    #print(f"Destination IPs: {set(dst_ips)}")
    print(f"IP Src, IP Dest: {tuplasDestSrc}")
    print(f"Total ICMP Packets:                {total_packets}")
    print(f"Taxa de Transf. Media(Throughput): {average_throughput:.2f} bytes/segundo")
    print(f"Intevalo Medio entre Packets:      {average_interval:.6f} segudos")
    
# Example usage
pcap_file = 'pacotes.pcapng'
extract_icmp_info(pcap_file)
